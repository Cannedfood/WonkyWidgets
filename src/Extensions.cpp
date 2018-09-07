#ifdef WWIDGET_LUA
	#define WWIDGET_LUA_IMPLEMENTATION
	#include "../include/wwidget-lua.hpp"
	extern "C" {
		int luaopen_wwidget(lua_State* L) {
			// TODO: package.loaded["wwidget"] = wwidget
			::wwidget::lua::openWwidget(L);
			return 1;
		}
	}
	#undef WWIDGET_LUA_IMPLEMENTATION
#endif
