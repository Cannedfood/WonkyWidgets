#pragma once

#include <wwidget/BasicContext.hpp>

#include <lua.hpp>

namespace wwidget {

class LuaContext : public BasicContext {
	lua_State* mState;
public:
	LuaContext();
	~LuaContext();

	void execute(Widget* at, std::string_view cmd) override;
	// void execute(Widget* at, std::string_view const* cmds, size_t count) override;
};

} // namespace wwidget