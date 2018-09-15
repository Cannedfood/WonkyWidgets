#pragma once

struct lua_State;

#include "wwidget/Widget.hpp"

namespace wwidget {

class Widget;
class Form;

} // namespace wwidget

namespace wwidget::lua {

void openWwidget(lua_State* state);

void pushWidget(lua_State* state, shared<Widget> widget);

} // namespace wwidget::lua

// =============================================================
// == Implementation =============================================
// =============================================================

#ifdef WWIDGET_LUA_IMPLEMENTATION

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include "wwidget.hpp"
#include "wwidget/widget/Text.hpp"

#include <cassert>

#define WWIDGET_LUA_SAFETY_CHECKS


namespace wwidget::lua::detail {

using namespace wwidget;
using namespace wwidget::lua;

class WwidgetLuaAttribute final : public wwidget::Attribute {
	lua_State* L;
	int        idx;
public:
	WwidgetLuaAttribute(lua_State* L, int position) : L(L), idx(position) {
		if(idx < 0)
			idx = lua_gettop(L) + 1 - idx;
	}

	Color toColor() const override {
		switch (lua_type(L, idx)) {
			case LUA_TSTRING: return wwidget::from_string<Color>(lua_tostring(L, idx));
			case LUA_TNUMBER: return Color((uint32_t)lua_tointeger(L, idx));
			default: throw std::runtime_error("Couldn't convert to color");
		}
	}
	Point toPoint() const override {
		switch(lua_type(L, idx)) {
			case LUA_TSTRING: return wwidget::from_string<Point>(lua_tostring(L, idx));
			case LUA_TTABLE: {
				Point result;
				lua_geti(L, idx, 1);
				result.x = lua_tonumber(L, -1);
				lua_geti(L, idx, 2);
				result.y = lua_tonumber(L, -1);
				lua_pop(L, 2);
				return result;
			}
			default: throw std::runtime_error("Couldn't convert to color");
		}
	}
	Offset  toOffset() const override { return Offset(toPoint()); }
	Size    toSize() const override { return Size(toPoint()); }
	Rect    toRect() const override {
		switch(lua_type(L, idx)) {
			case LUA_TSTRING: return wwidget::from_string<Rect>(lua_tostring(L, idx));
			case LUA_TTABLE: {
				lua_len(L, idx);
				int len = lua_tonumber(L, -1);
				lua_pop(L, 1);
				switch(len) {
					default: throw std::runtime_error("Expected a table with 2 or 4 entries");
					case 2: return Rect(toSize());
					case 4: {
						lua_geti(L, idx, 1);
						float x = lua_tonumber(L, -1);
						lua_geti(L, idx, 2);
						float y = lua_tonumber(L, -1);
						lua_geti(L, idx, 3);
						float w = lua_tonumber(L, -1);
						lua_geti(L, idx, 4);
						float h = lua_tonumber(L, -1);
						lua_pop(L, 4);
						return Rect(x, y, w, h);
					}
				}
			}
			default: throw std::runtime_error("Couldn't convert to color");
		}
	}
	Alignment toAlignment() const override {
		switch(lua_type(L, idx)) {
			case LUA_TSTRING: return wwidget::from_string<Alignment>(lua_tostring(L, idx));
			case LUA_TTABLE: {
				lua_geti(L, idx, 1);
				lua_geti(L, idx, 2);
				Alignment align = {
					wwidget::from_string<HalfAlignment>(lua_tostring(L, -2)),
					wwidget::from_string<HalfAlignment>(lua_tostring(L, -1))
				};
				lua_pop(L, 2);
				return align;
			}
			// TODO: accept table of two half alignments
			default: throw std::runtime_error("Expected alignment string");
		}
	}
	HalfAlignment toHalfAlignment() const override {
		if(lua_type(L, idx) != LUA_TSTRING) throw std::runtime_error("Expected alignment string");
		return wwidget::from_string<HalfAlignment>(lua_tostring(L, idx));
	}
	Padding toPadding() const override {
		switch(lua_type(L, idx)) {
			case LUA_TSTRING: return wwidget::from_string<Padding>(lua_tostring(L, idx));
			// TODO: accept table
			default: throw std::runtime_error("Expected alignment string");
		}
	}
	Flow toFlow() const override {
		if(lua_type(L, idx) != LUA_TSTRING) throw std::runtime_error("Expected flow string");
		return wwidget::from_string<Flow>(lua_tostring(L, idx));
	}

	std::string toString() const override { return lua_tostring(L, idx); }
	float       toFloat()  const override { return lua_tonumber(L, idx); }
	bool        toBool()   const override { return lua_toboolean(L, idx); }
	int64_t     toInt()    const override { return lua_tointeger(L, idx); }
};

struct LuaWidget {
	shared<Widget> pointer;
	size_t         checksum;

	constexpr static
	size_t const CHECKSUM_SEED = 0xB9A1499A11BAE09Alu;

	bool validate() const noexcept {
		return (checksum ^ CHECKSUM_SEED) == size_t(pointer.get());
	}

	void updateChecksum() noexcept {
		checksum = size_t(pointer.get()) ^ CHECKSUM_SEED;
	}
};

static
shared<Widget>& toWidget(lua_State* L, int index) {
	void* pointer = lua_touserdata(L, index);
	if(!pointer) {
		lua_pushfstring(L, "Expected %d to be userdata, got %s\n", index, lua_typename(L, index));
		lua_error(L);
	}
#ifdef WWIDGET_LUA_SAFETY_CHECKS
	auto& widget = *(LuaWidget*) pointer;
	if(!widget.validate()) {
		lua_pushfstring(L, "Invalid widget handle (%d on stack)", index);
		lua_error(L);
	}
	return widget.pointer;
#else
	return *(shared<Widget>*) pointer;
#endif
}

static
void pushWidget(lua_State* L, shared<Widget> w, int widget_table) {
	lua_pushvalue(L, widget_table);

#ifdef WWIDGET_LUA_SAFETY_CHECKS
	void* memory = lua_newuserdata(L, sizeof(LuaWidget));
	auto* widget = new(memory) LuaWidget;
	widget->pointer = std::move(w);
	widget->updateChecksum();
#else
	void* memory = lua_newuserdata(L, sizeof(shared<Widget>));
	auto* widget = new(memory) shared<Widget>(std::move(w));
#endif

	lua_pushvalue(L, -2);
	lua_setmetatable(L, -2);

	lua_replace(L, -2);
}

static
int widgetGC(lua_State* L) {
	auto& w = toWidget(L, 1);
	// printf("widgetGC %p (Refcount %i)\n", w.get(), w.refcount());
	w.~shared();
	return 0;
}

static
void widgetSetAttributeInternal(lua_State* L, Widget& self, int key, int value) {
	if(!lua_isstring(L, key)) {
		lua_pushstring(L, "Only strings are allowed as keys");
		lua_error(L);
	}

	const char* name = lua_tostring(L, key);
	if(strcmp("parent", name) == 0)
		toWidget(L, value)->add(self);
	else
		self.setAttribute(name, WwidgetLuaAttribute(L, value));
}

static
int widgetSetAttribute(lua_State* L) {
	auto& self = toWidget(L, 1);
	int args = lua_gettop(L);
	for(int i = 2; i <= args;) {
		if(lua_isstring(L, i)) {
			widgetSetAttributeInternal(L, *self, i, i+1);
			i += 2;
		}
		else if(lua_istable(L, i)) {
			lua_pushnil(L); // first key
      while(lua_next(L, i)) {
				widgetSetAttributeInternal(L, *self, lua_gettop(L) - 1, lua_gettop(L));

        // removes 'value'; keeps 'key' for next iteration
        lua_pop(L, 1);
      }
			lua_pop(L, 1);
			i += 1;
		}
		else {
			lua_pushstring(L, "widget.set: Expected table or string");
			lua_error(L);
		}
	}
	lua_settop(L, 1);
	return 1;
}

static
int widgetGetAttribute(lua_State* L) {
	// TODO
	lua_pushstring(L, "Unimplemented");
	lua_error(L);
	return 0;
}

static
int widgetListAttributes(lua_State* L) {
	// TODO
	lua_pushstring(L, "Unimplemented");
	lua_error(L);
	return 0;
}

static
int widgetAdd(lua_State* L) {
	int top = lua_gettop(L);
	if(top < 2) {
		lua_pushfstring(L, "widget:add(widgets...) expected at least one argument (and self)");
		lua_error(L);
	}

	auto& self = toWidget(L, 1);
	for(int arg = 2; arg <= top; arg++) {
		self->add(toWidget(L, arg));
	}

	return top - 1;
}

static
int widgetRemove(lua_State* L) {
	int top = lua_gettop(L);
	for(int i = 1; i <= top; i++) {
		auto& w = toWidget(L, i);
		w->remove();
	}
	return top;
}

// TODO: extract

static
int widgetClear(lua_State* L) {
	int top = lua_gettop(L);
	for(int i = 1; i <= top; i++) {
		toWidget(L, i)->clearChildren();
	}
	return top;
}

static
int widgetFindCLOSURE(lua_State* L) {
	auto& self = toWidget(L, 1);

	int top = lua_gettop(L);
	for(int i = 2; i <= top; i++) {
		const char* name = lua_tostring(L, i);
		shared<Widget> result = self->search(name);
		if(!result) lua_pushnil(L);
		else {
			pushWidget(L, result, lua_upvalueindex(1));
		}
	}

	return top - 1;
}

static
void makeClass(lua_State* L, int table, int inherit = 0) {
	assert(table > 0);
	assert(inherit >= 0);

#ifndef NDEBUG
	int top = lua_gettop(L);
#endif

	lua_pushvalue(L, table);
	lua_setfield(L, table, "__index");

	lua_newtable(L);
	int metatable = lua_gettop(L);

	lua_pushvalue(L, metatable);
	lua_setmetatable(L, table);

	lua_pushvalue(L, metatable);
	lua_setfield(L, table, "__meta");

	if(inherit != 0) {
		lua_pushvalue(L, inherit);
		lua_setfield(L, metatable, "__index");
	}

	lua_pushcfunction(L, [](lua_State* L) -> int {
		constexpr int self = 1;
		lua_getfield(L, self, "new");
		if(!lua_isfunction(L, -1)) {
			lua_pushstring(L, "Class has no function new to construct itself!");
			lua_error(L);
		}
		lua_insert(L, 2);
		lua_call(L, lua_gettop(L) - 2, 1);
		return 1;
	});
	lua_setfield(L, metatable, "__call");

	lua_pop(L, 1);

#ifndef NDEBUG
	assert(top == lua_gettop(L));
#endif
}

template<class T> static
int widgetNew(lua_State* L) {
	if(lua_gettop(L)) {
		lua_pushcfunction(L, widgetSetAttribute);
		lua_insert(L, 1);
		pushWidget(L, make_shared<T>(), lua_upvalueindex(1));
		lua_insert(L, 2);
		lua_call(L, lua_gettop(L) - 1, 1);
	}
	else {
		pushWidget(L, make_shared<T>(), lua_upvalueindex(1));
	}
	return 1;
}

template<class T> static
int widgetCast(lua_State* L) {
	int top = lua_gettop(L);
	for(int i = 1; i <= top; i++) {
		auto& p = toWidget(L, i);

		if(!p.template cast_dynamic<T>()) lua_pushnil(L);

		lua_pushvalue(L, lua_upvalueindex(1));
		lua_setmetatable(L, i);

		lua_pushvalue(L, i);
	}
	return top;
}

static
void pushWidgetClass(lua_State* L) {
	lua_newtable(L);
	int widget_class = lua_gettop(L);
	makeClass(L, widget_class);

	lua_pushcfunction(L, widgetSetAttribute); lua_setfield(L, widget_class, "set");
	lua_pushcfunction(L, widgetGetAttribute); lua_setfield(L, widget_class, "get");
	lua_pushcfunction(L, widgetListAttributes); lua_setfield(L, widget_class, "list");

	lua_pushcfunction(L, widgetAdd); lua_setfield(L, widget_class, "add");
	lua_pushcfunction(L, widgetRemove); lua_setfield(L, widget_class, "remove");
	lua_pushcfunction(L, widgetClear); lua_setfield(L, widget_class, "clear");

	lua_pushvalue(L, widget_class);
	lua_pushcclosure(L, widgetFindCLOSURE, 1); lua_setfield(L, widget_class, "find");

	lua_pushcfunction(L, widgetGC); lua_setfield(L, widget_class, "__gc");

	lua_pushvalue(L, widget_class);
	lua_pushcclosure(L, widgetNew<Widget>, 1); lua_setfield(L, widget_class, "new");
}

template<class T, class BaseT = void> static
void pushWidgetClass(lua_State* L, int base_class) {
	static_assert(
		std::is_same_v<BaseT, void> || std::is_base_of_v<BaseT, T>
	);

	lua_newtable(L);
	int w = lua_gettop(L);

	makeClass(L, w, base_class);

	lua_pushvalue(L, w);
	lua_pushcclosure(L, widgetNew<T>, 1);
	lua_setfield(L, w, "new");

	lua_pushvalue(L, w);
	lua_pushcclosure(L, widgetCast<T>, 1);
	lua_setfield(L, w, "cast");

	lua_pushcfunction(L, widgetGC);
	lua_setfield(L, w, "__gc");
}

} // namespace wwidget::lua::detail

void wwidget::lua::openWwidget(lua_State* L) {
	using namespace detail;

	lua_newtable(L);
	int lib = lua_gettop(L);

	lua_pushvalue(L, lib);
	lua_setglobal(L, "wwidget");

#define LIBMEMBER(name) \
	int name = lua_gettop(L); \
	lua_pushvalue(L, name); \
	lua_setfield(L, lib, #name)

	// Base widget
	pushWidgetClass(L);
	LIBMEMBER(widget);
	// Basic widgets
	pushWidgetClass<Text, Widget>(L, widget);
	LIBMEMBER(text);
	pushWidgetClass<TextField, Text>(L, text);
	LIBMEMBER(textfield);
	pushWidgetClass<Button, Widget>(L, widget);
	LIBMEMBER(button);
	pushWidgetClass<List, Widget>(L, widget);
	LIBMEMBER(list);
	pushWidgetClass<WrappedList, List>(L, list);
	LIBMEMBER(wrapped_list);
	pushWidgetClass<Slider, Widget>(L, widget);
	LIBMEMBER(slider);
	pushWidgetClass<Knob, Slider>(L, slider);
	LIBMEMBER(knob);
	pushWidgetClass<Image, Widget>(L, widget);
	LIBMEMBER(image);

	// #ifndef WWIDGET_NO_WINDOW
	// pushWidgetClass<Window, Widget>(L, widget);
	// LIBMEMBER(window);
	// #endif

#undef LIBMEMBER

	// lua_pushvalue(L, widget);
	// lua_setfield(L, lib, "widget");
	// lua_pushvalue(L, text); lua_setfield(L, lib, "text");

	lua_settop(L, lib - 1);
}

void wwidget::lua::pushWidget(lua_State* L, ::wwidget::shared<Widget> widget) {
	lua_getglobal(L, "wwidget");
	lua_getfield(L, -1, "widget");
	detail::pushWidget(L, widget, -1);
	lua_insert(L, -3);
	lua_pop(L, 2);
}

#endif // WWIDGET_LUA_IMPLEMENTATION
