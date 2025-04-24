#ifndef LUA_NETCLIENT_H
#define LUA_NETCLIENT_H
#include "platform/PlatformMacros.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "gameex/NetClient.h"
#include "gameex/NetMessage.h"

int lua_register_NetClient(lua_State* tolua_S);
#endif // LUA_NETCLIENT_H