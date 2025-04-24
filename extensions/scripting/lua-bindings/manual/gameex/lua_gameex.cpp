/****************************************************************************
   Copyright (c) 2013-2016 Chukong Technologies Inc.
   Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
   Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

   https://axmol.dev/

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
****************************************************************************/
#include "lua_gameex.h"
#include "lua_NetMessage.h"
#include "lua_NetClient.h"

// #include "lua_Bit6EnDecode.h"
// #include "lua_LuaNetworkController.h"
// #include "lua_GameEnvironment.h"
// #include "lua_luaSprite.h"

TOLUA_API int lua_register_gameex_Submodule(lua_State* tolua_S)
{
    AXLOGD("lua_register_gameex_module");
    //lua_register_EnDecode6BitStr(tolua_S);
    lua_register_NetMessage(tolua_S);
    lua_register_NetClient(tolua_S);
    AXLOGD("lua_register_myext_module ok");
    return 1;
}

TOLUA_API int register_type_gameex(lua_State* tolua_S)
{
    tolua_usertype(tolua_S, "netClient");
    tolua_usertype(tolua_S, "netMessage");
    // tolua_usertype(tolua_S, "netClient");
    // tolua_usertype(tolua_S, "netClient");
    // tolua_usertype(tolua_S, "netClient");
    AXLOGD("register_type registered in Lua");
    return 1;
}
TOLUA_API int register_gameex_module(lua_State* tolua_S)
{
    tolua_open(tolua_S);
    register_type_gameex(tolua_S);
    tolua_module(tolua_S, NULL, 0);
    tolua_beginmodule(tolua_S, NULL);
    lua_register_gameex_Submodule(tolua_S);
    tolua_endmodule(tolua_S);
    AXLOGD("gameex_module registered in Lua");
    return 1;
}