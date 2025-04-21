#ifndef LUA_NETMESSAGE_H
#define LUA_NETMESSAGE_H

#include "tolua++.h"
#include "platform/PlatformMacros.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "gameex/NetMessage.h"

int lua_register_ax_NetMessage(lua_State* tolua_S);

#endif // LUA_NETMESSAGE_H