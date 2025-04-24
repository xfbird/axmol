

#include "lua_NetClient.h"

// NetClient 构造函数
int lua_ax_NetClient_constructor(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_constructor called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    argc = lua_gettop(tolua_S) - 1;
    AXLOGD("lua_ax_NetClient_constructor argc:{}", argc);
    if (argc == 0)
    {
        cobj = ax::gameex::NetClient::create();
        AXLOGD("lua_ax_NetClient_constructor create cobj:{:12X}", FMT_TOPOINT(cobj));
        if (cobj)
        {
            AXLOGD("NetClient instance created ok cobj:{:12X}", FMT_TOPOINT(cobj));
            tolua_pushusertype(tolua_S, (void*)cobj, "netClient");
            // tolua_register_gc(tolua_S, lua_gettop(tolua_S));
            AXLOGD("NetClient instance pushed to Lua stack ok");
            return 1;
        }
        lua_pushnil(tolua_S);
        AXLOGD("NetClient instance pushed to Lua stack fail");
        return 0;
    }
    AXLOGD("lua_ax_NetClient_constructor failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:NetClient", argc, 0);
    return 0;
}
int lua_ax_NetClient_deconstructor(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_deconstructor called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
    AXLOGD("NetClient deconstructor GetArg cobj:{:12X}", FMT_TOPOINT(cobj));
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_disconnect'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_disconnect");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        cobj->destroyInstance();
        AXLOGD("NetClient deconstructor ");
        return 1;
    }
    AXLOGD("lua_ax_NetClient_deconstructor failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:disconnect", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_deconstructor'.", &tolua_err);
#endif
    return 0;
}
// NetClient 析构函数
static int lua_ax_NetClient_finalize(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_finalize called");
    AXLOGV("luabindings: finalizing LUA object (NetClient)");
    return 0;
}
// 连接到服务器
int lua_ax_NetClient_connect(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_connect called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
    AXLOGD("lua_ax_NetClient_connect GetArg cobj:{:12X}", FMT_TOPOINT(cobj));
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_connect'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_connect");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2)
    {
        std::string srvip;
        int srvPort;
        ok &= luaval_to_std_string(tolua_S, 2, &srvip, "ax.NetClient:connect");
        ok &= luaval_to_int32(tolua_S, 3, (int*)&srvPort, "ax.NetClient:connect");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_NetClient_connect'", nullptr);
            AXLOGD("Invalid arguments in lua_ax_NetClient_connect");
            return 0;
        }
        bool ret = cobj->Connect(srvip, srvPort);
        AXLOGD("Connecting to server: {}:{} ", srvip, srvPort);
        if (ret)
        {
            tolua_pushnumber(tolua_S, (lua_Number)1);
            return 1;
        }
        tolua_pushnumber(tolua_S, (lua_Number)0);
        return 0;
    }
    AXLOGD("lua_ax_NetClient_connect failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:connect", argc, 2);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_connect'.", &tolua_err);
#endif
    return 0;
}
// 断开连接
int lua_ax_NetClient_disconnect(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_disconnect called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
    AXLOGD("lua_ax_NetClient_disconnect GetArg cobj:{:12X}", FMT_TOPOINT(cobj));
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_disconnect'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_disconnect");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    AXLOGD("lua_ax_NetClient_disconnect argc:{}", argc);
    if (argc == 0)
    {
        AXLOGD("lua_ax_NetClient_disconnect cobj:{:12X} Disconnect", FMT_TOPOINT(cobj));
        cobj->Close();
        // Disconnect();
        AXLOGD("Disconnecting from server");
        return 1;
    }
    AXLOGD("lua_ax_NetClient_disconnect failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:disconnect", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_disconnect'.", &tolua_err);
#endif
    return 0;
}
int lua_ax_NetClient_CleanAllMessageHandler(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_CleanAllMessageHandler called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
    AXLOGD("lua_ax_NetClient_CleanAllMessageHandler GetArg cobj:{:12X}", FMT_TOPOINT(cobj));
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_CleanAllMessageHandler'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_CleanAllMessageHandler");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    AXLOGD("lua_ax_NetClient_CleanAllMessageHandler argc:{}", argc);
    if (argc == 0)
    {
        AXLOGD("lua_ax_NetClient_CleanAllMessageHandler cobj:{:12X} Clear", FMT_TOPOINT(cobj));
        cobj->CleanTcpSink();
        // Disconnect();
        AXLOGD("CleanAllMessageHandler for netclient");
        return 1;
    }
    AXLOGD("lua_ax_NetClient_disconnect failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:CleanAllMessageHandler", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_CleanAllMessageHandler'.", &tolua_err);
#endif
    return 0;
}
// 发送消息
int lua_ax_NetClient_sendMsg(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_sendMsg called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_sendMsg'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_sendMsg");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        ax::gameex::NetMessage* msg;
        ok &= luaval_to_object<ax::gameex::NetMessage>(tolua_S, 2, "ax.gameex::NetMessage", &msg);
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_NetClient_sendMsg'", nullptr);
            AXLOGD("Invalid arguments in lua_ax_NetClient_sendMsg");
            return 0;
        }
        cobj->SendMsg(msg);
        AXLOGD("Sending message: {}", msg->GetMsgID());
        return 1;
    }
    AXLOGD("lua_ax_NetClient_sendMsg failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:sendMsg", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_sendMsg'.", &tolua_err);
#endif
    return 0;
}
// 检查连接状态
int lua_ax_NetClient_isConnected(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_isConnected called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_isConnected'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_isConnected");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        bool ret = cobj->IsConnected();
        tolua_pushboolean(tolua_S, ret);
        AXLOGD("IsConnected returned: {}", ret);
        return 1;
    }
    AXLOGD("lua_ax_NetClient_isConnected failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:isConnected", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_isConnected'.", &tolua_err);
#endif
    return 0;
}
// 获取 ServerKey
int lua_ax_NetClient_getServerKey(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_getServerKey called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_getServerKey'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_getServerKey");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        std::string ret = cobj->get_ServerKey();
        tolua_pushstring(tolua_S, ret.c_str());
        AXLOGD("ServerKey retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_ax_NetClient_getServerKey failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:getServerKey", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_getServerKey'.", &tolua_err);
#endif
    return 0;
}
// 设置 ServerKey
int lua_ax_NetClient_setServerKey(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_setServerKey called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_setServerKey'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_setServerKey");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.NetClient:setServerKey");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_NetClient_setServerKey'", nullptr);
            AXLOGD("Invalid arguments in lua_ax_NetClient_setServerKey");
            return 0;
        }
        cobj->set_ServerKey(arg0);
        AXLOGD("ServerKey set to: {}", arg0);
        return 1;
    }
    AXLOGD("lua_ax_NetClient_setServerKey failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:setServerKey", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_setServerKey'.", &tolua_err);
#endif
    return 0;
}
// 获取 TypeData
int lua_ax_NetClient_getTypeData(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_getTypeData called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_getTypeData'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_getTypeData");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        int ret = cobj->get_TypeData();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        AXLOGD("TypeData retrieved: {}", ret);
        return 1;
    }
    AXLOGD("lua_ax_NetClient_getTypeData failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:getTypeData", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_getTypeData'.", &tolua_err);
#endif
    return 0;
}
// 设置 TypeData
int lua_ax_NetClient_setTypeData(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_setTypeData called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_setTypeData'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_setTypeData");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2, (int*)&arg0, "ax.NetClient:setTypeData");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_NetClient_setTypeData'", nullptr);
            AXLOGD("Invalid arguments in lua_ax_NetClient_setTypeData");
            return 0;
        }
        cobj->set_TypeData(arg0);
        AXLOGD("TypeData set to: {}", arg0);
        return 1;
    }
    AXLOGD("lua_ax_NetClient_setTypeData failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NetClient:setTypeData", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_setTypeData'.", &tolua_err);
#endif
    return 0;
}
int lua_ax_NetClient_Tick(lua_State* tolua_S)
{
    AXLOGD("lua_ax_NetClient_Tick called");
    int argc                    = 0;
    ax::gameex::NetClient* cobj = nullptr;
    bool ok                     = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "netClient", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::gameex::NetClient*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_NetClient_Tick'", nullptr);
        AXLOGD("Invalid 'cobj' in lua_ax_NetClient_Tick");
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    // if (true)
    {
        cobj->Tick();
        AXLOGD("lua_ax_NetClient_Tick: ok ");
        return 1;
    }
    AXLOGD("lua_ax_NetClient_Tick failed: wrong number of arguments");
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "lua_ax_NetClient_Tick", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_NetClient_connect'.", &tolua_err);
#endif
    return 0;
}

int lua_register_NetClient(lua_State* tolua_S)
{
    tolua_cclass(tolua_S, "netClient", "netClient", "ax.Object", NULL);
    tolua_beginmodule(tolua_S, "netClient");
    tolua_function(tolua_S, "new", lua_ax_NetClient_constructor);       // 构造
    tolua_function(tolua_S, "create", lua_ax_NetClient_deconstructor);  // 创建 如果不被Lua 集成就可以有
    tolua_function(tolua_S, "Tick", lua_ax_NetClient_Tick);
    tolua_function(tolua_S, "Connect", lua_ax_NetClient_connect);        // 连接
    tolua_function(tolua_S, "Disconnect", lua_ax_NetClient_disconnect);  // 断开
    tolua_function(tolua_S, "SendNetMessage", lua_ax_NetClient_sendMsg);
    tolua_function(tolua_S, "IsConnected", lua_ax_NetClient_isConnected);
    tolua_function(tolua_S, "GetNetServerKey", lua_ax_NetClient_getServerKey);
    tolua_function(tolua_S, "SetNetServerKey", lua_ax_NetClient_setServerKey);
    tolua_function(tolua_S, "GetNetMessageTypeSData", lua_ax_NetClient_getTypeData);
    tolua_function(tolua_S, "SetNetMessageTypeSData", lua_ax_NetClient_setTypeData);
    tolua_function(tolua_S, "CleanAllMessageHandler", lua_ax_NetClient_CleanAllMessageHandler);  // 清除所有的消息处理句柄
    tolua_endmodule(tolua_S);
    auto typeName                                    = typeid(ax::gameex::NetClient).name();  // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "netClient";
    g_typeCast[typeName]                             = "netClient";
    AXLOGD("netclient registered in Lua ok");
    return 1;
}

// int register_NetClient(lua_State* tolua_S)
// {
//     tolua_open(tolua_S);
//     tolua_usertype(tolua_S, "netClient");
//     tolua_module(tolua_S, NULL, 0);
//     tolua_beginmodule(tolua_S, NULL);
//     lua_register_NetClient_Sub(tolua_S);
//     tolua_endmodule(tolua_S);
//     AXLOGD("NetClient registered in Lua");
//     return 1;
// }