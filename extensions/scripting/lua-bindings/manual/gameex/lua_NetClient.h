#ifndef LUA_NETCLIENT_H
#define LUA_NETCLIENT_H
#include "platform/PlatformMacros.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
//#include "tolua++.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "gameex/NetClient.h"
#include "gameex/NetMessage.h"

// namespace ax{
int lua_register_ax_NetClient(lua_State* tolua_S);
// int lua_register_type_ax_NetClient(lua_State* tolua_S);
// }
#endif // LUA_NETCLIENT_H