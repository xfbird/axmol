#ifndef LUA_register_gameex_H
#define LUA_register_gameex_H

#include "tolua++.h"
#include "platform/PlatformMacros.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

TOLUA_API int register_gameex_module(lua_State* tolua_S);

#endif  // LUA_register_myext_H