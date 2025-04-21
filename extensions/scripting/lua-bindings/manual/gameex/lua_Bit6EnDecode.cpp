
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua_Bit6EnDecode.h"

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
        std::string ret = ax::gameex::Encode6BitStr(arg0);
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
        AXLOGD("Encode6BitStr source:{}", arg0);
        std::string ret = ax::gameex::Decode6BitStr(arg0);
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
static luaL_Reg lua_EnDeCodeexts[] = {{"Encode6BitBuf", lua_Encode6BitStr}, {"Decode6BitBuf", lua_Decode6BitStr}, {NULL, NULL}};
TOLUA_API int lua_register_EnDecode6BitStr(lua_State* L)
{
    // AXLOGD("lua_register_Decode6BitStr Call");
    luaL_register(L, "_G", lua_EnDeCodeexts);
    return 1;
}
