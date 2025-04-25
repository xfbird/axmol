#include "lua-bindings/manual/gameex/lua_NetMessage.h"

// // NetMessage 构造函数
// int lua_NetMessage_constructor(lua_State* tolua_S)
// {
//     AXLOGD("lua_NetMessage_constructor called");
//     int argc                     = 0;
//     ax::gameex::NetMessage* cobj = nullptr;
//     bool ok                      = true;

// #if _AX_DEBUG >= 1
//     tolua_Error tolua_err;
// #endif

//     argc = lua_gettop(tolua_S) - 1;
//     AXLOGD("lua_NetMessage_constructor argc:{}", argc);
//     if (argc == 0)
//     {
//         if (!ok)
//         {
//             tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_constructor'", nullptr);
//             AXLOGD("Invalid arguments in lua_NetMessage_constructor");
//             return 0;
//         }
//         cobj = new ax::gameex::NetMessage();
//         if (cobj)
//         {
//             cobj->autorelease();
//             tolua_pushusertype(tolua_S, (void*)cobj, "netMessage");
//             // tolua_register_gc(tolua_S, lua_gettop(tolua_S));
//             // AXLOGD("NetMessage instance pushed to Lua stack");
//             AXLOGD("NetMessage instance created Cobj {}", FMT_TOPOINT(cobj));
//             return 1;
//         }
//         AXLOGW("NetMessage instance created Cobj Faile");
//         return 0;
//     }

//     AXLOGD("lua_NetMessage_constructor failed: wrong number of arguments");
//     luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:NetMessage", argc, 0);
//     return 0;
//     #if _AX_DEBUG >= 1
//     tolua_lerror:
//         tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_GetHeader'.", &tolua_err);
//     #endif
//     return 0;
// }
int lua_NetMessage_constructor(lua_State* tolua_S)
{
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::gameex::NetMessage();
        if (cobj)
        {
            cobj->autorelease();
            object_to_luaval<ax::gameex::NetMessage>(tolua_S, "NetMessage", (ax::gameex::NetMessage*)cobj);
            return 1;
        }
        AXLOGW("lua_NetMessage_constructor Fail");
        return 0;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d ", "ax.NetMessage", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_constructor'.", &tolua_err);
#endif
    return 0;
}

int lua_netMessage_ToNetMessageHeader(lua_State* tolua_S, ax::gameex::NetMessage* msg)
{
    if (tolua_S)
    {
        lua_createtable(tolua_S, 0, 0);
        lua_pushstring(tolua_S, "recog");
        lua_pushnumber(tolua_S, msg->GetRecog());
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "msgId");
        lua_pushnumber(tolua_S, msg->GetMsgID());
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "param1");
        lua_pushnumber(tolua_S, msg->GetParam1());
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "param2");
        lua_pushnumber(tolua_S, msg->GetParam2());
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "param3");
        lua_pushnumber(tolua_S, msg->GetParam3());
        lua_rawset(tolua_S, -3);
        lua_pushstring(tolua_S, "param4");
        lua_pushnumber(tolua_S, msg->GetParam4());
        lua_rawset(tolua_S, -3);
        return 1;
    }
    return 0;
}

// 获取消息 ID
int lua_NetMessage_GetHeader(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_GetHeader called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);
    AXLOGD("lua_NetMessage_GetHeader Get Cobj:{}", FMT_TOPOINT(cobj));
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_GetHeader'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_GetHeader");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    AXLOGD("lua_NetMessage_GetHeader Get argc:{}", argc);
    if (argc == 0)
    {
        // int32_t ret = cobj->GetMsgID();
        // AXLOGD("lua_NetMessage_GetHeader Get GetMsgID:{}", ret);
        // tolua_pushnumber(tolua_S, (lua_Number)ret);
        // AXLOGD("Header retrieved: {}", ret);
        // return 1;
        return lua_netMessage_ToNetMessageHeader(tolua_S, cobj);
    }
    AXLOGD("lua_NetMessage_GetHeader failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:GetHader", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_GetHeader'.", &tolua_err);
#endif
    return 0;
}

// 获取消息 ID
int lua_NetMessage_getMsgID(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getMsgID called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);
    AXLOGD("lua_NetMessage_getMsgID Get Cobj:{}", FMT_TOPOINT(cobj));
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getMsgID'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getMsgID");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    AXLOGD("lua_NetMessage_getMsgID Get argc:{}", argc);
    if (argc == 0)
    {
        int32_t ret = cobj->GetMsgID();
        AXLOGD("lua_NetMessage_getMsgID Get GetMsgID:{}", ret);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("MsgID retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getMsgID failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getMsgID", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getMsgID'.", &tolua_err);
#endif

    return 0;
}

// 设置消息 ID
int lua_NetMessage_setMsgID(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_setMsgID called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_setMsgID'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_setMsgID");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int32_t arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int32_t*)&arg0, "ax.netMessage:setMsgID");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_setMsgID'", nullptr);
            AXLOGD("Invalid arguments in lua_NetMessage_setMsgID");
            return 0;
        }
        cobj->SetMsgID(arg0);
        AXLOGD("MsgID set to: {}", arg0);
        return 1;
    }
    AXLOGD("lua_NetMessage_setMsgID failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:setMsgID", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_setMsgID'.", &tolua_err);
#endif

    return 0;
}

// 获取 Recog
int lua_NetMessage_getRecog(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getRecog called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getRecog'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getRecog");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int32_t ret = cobj->GetRecog();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("Recog retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getRecog failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getRecog", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getRecog'.", &tolua_err);
#endif

    return 0;
}

// 设置 Recog
int lua_NetMessage_setRecog(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_setRecog called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_setRecog'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_setRecog");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int32_t arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int32_t*)&arg0, "ax.netMessage:setRecog");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_setRecog'", nullptr);
            AXLOGD("Invalid arguments in lua_NetMessage_setRecog");
            return 0;
        }
        cobj->SetRecog(arg0);
        AXLOGD("Recog set to: {}", arg0);
        return 1;
    }
    AXLOGD("lua_NetMessage_setRecog failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:setRecog", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_setRecog'.", &tolua_err);
#endif

    return 0;
}

// 获取 Param1
int lua_NetMessage_getParam1(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getParam1 called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getParam1'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getParam1");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int32_t ret = cobj->GetParam1();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("Param1 retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getParam1 failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getParam1", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getParam1'.", &tolua_err);
#endif

    return 0;
}

// 设置 Param1
int lua_NetMessage_setParam1(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_setParam1 called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_setParam1'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_setParam1");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int32_t arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int32_t*)&arg0, "ax.netMessage:setParam1");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_setParam1'", nullptr);
            AXLOGD("Invalid arguments in lua_NetMessage_setParam1");
            return 0;
        }
        cobj->SetParam1(arg0);
        AXLOGD("Param1 set to: {}", arg0);
        return 1;
    }
    AXLOGD("lua_NetMessage_setParam1 failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:setParam1", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_setParam1'.", &tolua_err);
#endif

    return 0;
}

// 获取 Param2
int lua_NetMessage_getParam2(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getParam2 called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getParam2'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getParam2");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int32_t ret = cobj->GetParam2();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("Param2 retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getParam2 failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getParam2", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getParam2'.", &tolua_err);
#endif

    return 0;
}

// 设置 Param2
int lua_NetMessage_setParam2(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_setParam2 called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_setParam2'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_setParam2");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int32_t arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int32_t*)&arg0, "ax.netMessage:setParam2");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_setParam2'", nullptr);
            AXLOGD("Invalid arguments in lua_NetMessage_setParam2");
            return 0;
        }
        cobj->SetParam2(arg0);
        AXLOGD("Param2 set to: {}", arg0);
        return 1;
    }
    AXLOGD("lua_NetMessage_setParam2 failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:setParam2", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_setParam2'.", &tolua_err);
#endif

    return 0;
}

// 获取 Param3
int lua_NetMessage_getParam3(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getParam3 called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getParam3'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getParam3");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int32_t ret = cobj->GetParam3();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("Param3 retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getParam3 failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getParam3", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getParam3'.", &tolua_err);
#endif

    return 0;
}

// 设置 Param3
int lua_NetMessage_setParam3(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_setParam3 called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_setParam3'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_setParam3");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int32_t arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int32_t*)&arg0, "ax.netMessage:setParam3");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_setParam3'", nullptr);
            AXLOGD("Invalid arguments in lua_NetMessage_setParam3");
            return 0;
        }
        cobj->SetParam3(arg0);
        AXLOGD("Param3 set to: {}", arg0);
        return 1;
    }
    AXLOGD("lua_NetMessage_setParam3 failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:setParam3", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_setParam3'.", &tolua_err);
#endif

    return 0;
}

// 获取 Param4
int lua_NetMessage_getParam4(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getParam4 called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getParam4'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getParam4");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int32_t ret = cobj->GetParam4();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("Param4 retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getParam4 failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getParam4", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getParam4'.", &tolua_err);
#endif

    return 0;
}

int lua_NetMessage_getLength(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getLength called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getLength'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getLength");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int32_t ret = cobj->GetLength();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("Length retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getLength failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getLength", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getLength'.", &tolua_err);
#endif

    return 0;
}
int lua_NetMessage_getIsZlib(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getIsZlib called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getIsZlib'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getIsZlib");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int32_t ret = cobj->GetIsZlib();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("IsZlib retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getIsZlib failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getIsZlib", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getIsZlib'.", &tolua_err);
#endif

    return 0;
}

int lua_NetMessage_getIndex(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getIndex called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getIndex'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getIndex");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int32_t ret = cobj->GetIndex();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("Index retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getIndex failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getIndex", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getIndex'.", &tolua_err);
#endif

    return 0;
}
// 设置 Param4
int lua_NetMessage_setParam4(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_setParam4 called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_setParam4'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_setParam4");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int32_t arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int32_t*)&arg0, "ax.netMessage:setParam4");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_setParam4'", nullptr);
            AXLOGD("Invalid arguments in lua_NetMessage_setParam4");
            return 0;
        }
        cobj->SetParam4(arg0);
        AXLOGD("Param4 set to: {}", arg0);
        return 1;
    }
    AXLOGD("lua_NetMessage_setParam4 failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:setParam4", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_setParam4'.", &tolua_err);
#endif

    return 0;
}

// 获取消息内容
int lua_NetMessage_getData(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getData called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getData'", nullptr);
            AXLOGD("Invalid 'cobj' in lua_NetMessage_getData");
            return 0;
        }
    #endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        object_to_luaval<ax::gameex::NetMessage>(tolua_S, "NetMessage", (ax::gameex::NetMessage*)cobj);
        AXLOGD("getData retrieved: {:12X}",FMT_TOPOINT(cobj));
        return 1;
    }
    AXLOGD("lua_NetMessage_getData failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getData", argc, 0);
    return 0;
    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getStrInfo'.", &tolua_err);
    #endif
    return 0;
}

// 获取消息内容
int lua_NetMessage_getStrInfo(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_getStrInfo called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_getStrInfo'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_getStrInfo");
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        std::string_view ret = cobj->GetStrInfo();
        lua_pushlstring(tolua_S, ret.data(), ret.size());
        // tolua_pushlstring(tolua_S, ret.data(), ret.size());
        AXLOGD("StrInfo retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_NetMessage_getStrInfo failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:getStrInfo", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_getStrInfo'.", &tolua_err);
#endif

    return 0;
}

// 设置消息内容
int lua_NetMessage_setStrInfo(lua_State* tolua_S)
{
    AXLOGD("lua_NetMessage_setStrInfo called");
    int argc                     = 0;
    ax::gameex::NetMessage* cobj = nullptr;
    bool ok                      = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netMessage", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetMessage*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_NetMessage_setStrInfo'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_NetMessage_setStrInfo");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.netMessage:setStrInfo");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_NetMessage_setStrInfo'", nullptr);
            AXLOGD("Invalid arguments in lua_NetMessage_setStrInfo");
            return 0;
        }
        cobj->SetStrInfo(arg0);
        AXLOGD("StrInfo set to: {}", arg0);
        tolua_pushboolean(tolua_S, 1);
        return 1;
    }
    AXLOGD("lua_NetMessage_setStrInfo failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.netMessage:setStrInfo", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_NetMessage_setStrInfo'.", &tolua_err);
#endif
    return 0;
}

static int lua_NetMessage_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (NetMessage)");
    return 0;
}
// WriteHeader
// GetHeader
// WriteData
// GetData
// GetDataLength
// ClearData

int lua_register_NetMessage(lua_State* tolua_S)
{
    tolua_cclass(tolua_S, "netMessage", "netMessage", "ax.Object", NULL);
    tolua_beginmodule(tolua_S, "netMessage");
    tolua_function(tolua_S, "new", lua_NetMessage_constructor);
    tolua_function(tolua_S, "getMsgID", lua_NetMessage_getMsgID);
    tolua_function(tolua_S, "SetHeaderMsgId", lua_NetMessage_setMsgID);
    tolua_function(tolua_S, "getRecog", lua_NetMessage_getRecog);
    tolua_function(tolua_S, "SetHeaderRecog", lua_NetMessage_setRecog);
    tolua_function(tolua_S, "getParam1", lua_NetMessage_getParam1);
    tolua_function(tolua_S, "SetHeaderParam1", lua_NetMessage_setParam1);
    tolua_function(tolua_S, "getParam2", lua_NetMessage_getParam2);
    tolua_function(tolua_S, "SetHeaderParam2", lua_NetMessage_setParam2);
    tolua_function(tolua_S, "getParam3", lua_NetMessage_getParam3);
    tolua_function(tolua_S, "SetHeaderParam3", lua_NetMessage_setParam3);
    tolua_function(tolua_S, "getParam4", lua_NetMessage_getParam4);
    tolua_function(tolua_S, "SetHeaderParam4", lua_NetMessage_setParam4);

    tolua_function(tolua_S, "GetDataLength", lua_NetMessage_getLength);
    tolua_function(tolua_S, "getDataLength", lua_NetMessage_getLength);
    tolua_function(tolua_S, "getLength", lua_NetMessage_getLength);
    tolua_function(tolua_S, "getIsZlib", lua_NetMessage_getIsZlib);
    tolua_function(tolua_S, "getIndex", lua_NetMessage_getIndex);

    tolua_function(tolua_S, "ReadString", lua_NetMessage_getStrInfo);

    tolua_function(tolua_S, "GetData", lua_NetMessage_getData);
    tolua_function(tolua_S, "getData", lua_NetMessage_getData);
    tolua_function(tolua_S, "WriteData", lua_NetMessage_setStrInfo);
    tolua_function(tolua_S, "writeData", lua_NetMessage_setStrInfo);
    tolua_function(tolua_S, "GetHeader", lua_NetMessage_GetHeader);
    // tolua_function(tolua_S, "destory", lua_NetMessage_Destory);
    tolua_endmodule(tolua_S);
    auto typeName                                    = typeid(ax::gameex::NetMessage).name();  // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "netMessage";
    g_typeCast[typeName]                             = "netMessage";
    return 1;
}

// int lua_register_NetMessage(lua_State* tolua_S)
// {
//     tolua_open(tolua_S);
//     tolua_usertype(tolua_S, "netMessage");
//     tolua_module(tolua_S, NULL, 0);
//     tolua_beginmodule(tolua_S, NULL);
//     lua_register_NetMessage_Sub(tolua_S);
//     tolua_endmodule(tolua_S);
//     AXLOGD("NetMessage registered in Lua");
//     return 1;
// }
