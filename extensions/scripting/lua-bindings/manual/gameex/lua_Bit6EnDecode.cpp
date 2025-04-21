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
// #include "lua-bindings/manual/gmxxext/lua_Reg_myext.h"
#include "lua_Bit6EnDecode.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/tolua_fix.h"
int lua_Encode6BitStr(lua_State* tolua_S)
{
    // AXLOGD("lua_Encode6BitStr called");
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
        tolua_Error tolua_err;
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
     {
         std::string arg0;
         ok &= luaval_to_std_string(tolua_S, 1, &arg0, "lua_Encode6BitStr");
         if (!ok)
          {
              tolua_error(tolua_S, "invalid arguments in function 'lua_Encode6BitStr'", nullptr);
              AXLOGD("Invalid arguments in lua_Encode6BitStr");
              return 0;
          }
         AXLOGD("Encode6BitStr source:{}", arg0);
         std::string ret=ax::gameex::Encode6BitStr(arg0);
         AXLOGD("Encode6BitStr ret: {}", ret);
         tolua_pushstring(tolua_S, ret.c_str());
         return 1;
     }
    AXLOGD("lua_Encode6BitStr failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "lua_Encode6BitStr", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
        tolua_error(tolua_S, "#ferror in function 'lua_Encode6BitStr'.", &tolua_err);
#endif
    return 0;
}
// 设置 ServerKey
int lua_Decode6BitStr(lua_State* tolua_S)
{
    AXLOGD("lua_Decode6BitStr called");
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
        tolua_Error tolua_err;
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
     {
         std::string arg0;
         ok &= luaval_to_std_string(tolua_S, 1, &arg0, "lua_Decode6BitStr");
         if (!ok)
          {
              tolua_error(tolua_S, "invalid arguments in function 'lua_Decode6BitStr'", nullptr);
              AXLOGD("Invalid arguments in lua_Decode6BitStr");
              return 0;
          }
         AXLOGD("Encode6BitStr source:{}",arg0);
         std::string ret=ax::gameex::Decode6BitStr(arg0);
         AXLOGD("Encode6BitStr ret:{}", ret);
         tolua_pushstring(tolua_S, ret.c_str());
         return 1;
     }
    AXLOGD("lua_Decode6BitStr failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "lua_Encode6BitStr", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
        tolua_error(tolua_S, "#ferror in function 'lua_Decode6BitStr'.", &tolua_err);
#endif
    return 0;
}
static luaL_Reg lua_EnDeCodeexts[] = {
    {"Encode6BitBuf", lua_Encode6BitStr},
    {"Decode6BitBuf", lua_Decode6BitStr},
    {NULL, NULL}
};
TOLUA_API int lua_register_EnDecode6BitStr(lua_State* L)
{
    AXLOGD("lua_register_Decode6BitStr Call");
    // lua_getglobal(L, "package");
    // lua_getfield(L, -1, "preload");
    // auto lib = lua_EnDeCodeexts;
    // for (; lib->func; ++lib)
    // {
    //     lua_pushcfunction(L, lib->func);
    //     lua_setfield(L, -2, lib->name);
    // }
    // lua_pop(L, 2);
    luaL_register(L, "_G", lua_EnDeCodeexts);
    return 1;
}
// TOLUA_API int lua_register_myext_module(lua_State* L)
// {
//     lua_register_EnDecode6BitStr(L);
//     AXLOGD("lua_register_myext_module");
//     lua_getglobal(L, "_G");
//     if (lua_istable(L, -1))  // stack:...,_G,
//     {
//         tolua_open(L);
//         tolua_module(L, "ax", 0);
//         tolua_beginmodule(L, "ax");
//             lua_register_ax_HttpDownloader(L);
//             lua_register_ax_LuaNetworkController(L);
//             lua_register_ax_NetClient(L);
//             lua_register_ax_GameEnvironment(L);
//             lua_register_ax_LuaSprite(L);
//         tolua_endmodule(L);
//     }
//     lua_pop(L, 1);
//     AXLOGD("lua_register_myext_module ok");
//     return 1;
// }
// NS_AX_END
