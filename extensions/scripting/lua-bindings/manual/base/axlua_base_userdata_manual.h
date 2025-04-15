#pragma once
#ifndef __AXLUA_BASE_USERDATA_MANUAL_H__
#define __AXLUA_BASE_USERDATA_MANUAL_H__

#include "axmol.h"
#include "base/UserData.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

using namespace ax;
//{
int register_userdata_module(lua_State* tolua_S);

//} // namespace ax

#endif  // __AXLUA_BASE_USERDATA_MANUAL_H__