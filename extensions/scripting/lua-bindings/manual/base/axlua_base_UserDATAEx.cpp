
#include "lua-bindings/manual/base/axlua_base_UserDATAEx.hpp"
#include "base/UserDATAEx.h"
// #include "2d/TileMapManager.h"
#include "base/mTileMapManager.h"

#include "axmol.h"
// #include "2d/Node.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "UserDataEx.h"
#include "base/Logging.h"
#include "tolua++.h"

using namespace axmol;
int lua_ax_base_UserDataEx_saveData(lua_State* tolua_S)
{
    int argc = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_saveData'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        // Since saveData doesn't take any parameters, we can directly call it.
        bool ret = cobj->checkAndSave();
        tolua_pushboolean(tolua_S, ret ? 1 : 0); // Push the boolean result onto the Lua stack.
        return 1; // Return the number of values pushed onto the stack.
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:saveData", argc, 0);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_saveData'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_setDoubleForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
            if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
        // //AXLOGD("lua_ax_base_UserDataEx_setDoubleForKey cobj:{}",fmt::ptr(cobj));
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_setDoubleForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        // //AXLOGD("lua_ax_base_UserDataEx_setDoubleForKey cobj:{} argc:{}",fmt::ptr(cobj),argc);
        do
        {
            if (argc == 2)
            {
                std::string arg0;
                double arg1 = 0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:setDoubleForKey");
                // //AXLOGD("lua_ax_base_UserDataEx_setDoubleForKey cobj:{}  luaval_to_std_string arg0  ok:{} arg0:{}",fmt::ptr(cobj),ok,arg0);
                ok &= luaval_to_number(tolua_S, 3, &arg1, "UserDataEx:setDoubleForKey");
                // //AXLOGD("lua_ax_base_UserDataEx_setDoubleForKey luaval_to_number  cobj:{}  arg1  ok:{}   arg1:{}",fmt::ptr(cobj),ok,arg1);
                if (!ok)
                {
                    break;
                }
                // //AXLOGD("lua_ax_base_UserDataEx_setDoubleForKey cobj:{} arg0:{} arg1:{}",fmt::ptr(cobj),arg0,arg1);
                cobj->setDoubleForKey(arg0, arg1);
                // //AXLOGD("lua_ax_base_UserDataEx_setDoubleForKey setDoubleForKey end");
                return 0;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:setDoubleForKey", argc, 2);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_setDoubleForKey'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_getDoubleForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
            if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
    cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    //AXLOGD("lua_ax_base_UserDataEx_getDoubleForKey 1 cobj:{}",fmt::ptr(cobj));
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_getDoubleForKey'", nullptr);
            return 0;
        }
    #endif    
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_getDoubleForKey 2 argc:{}",argc);
        do
        {
            if (argc == 1)
            {
                std::string arg0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:getDoubleForKey");
                //AXLOGD("lua_ax_base_UserDataEx_getDoubleForKey 3 arg0:{}   ok:{}",arg0,ok);
                if (!ok)
                {
                    break;
                }
                //AXLOGD("lua_ax_base_UserDataEx_getDoubleForKey 4 Exec cobj:{}  getDoubleForKey(arg0:{})",
                //             fmt::ptr(cobj),arg0,ok);
                double ret = cobj->getDoubleForKey(arg0);
                //AXLOGD("lua_ax_base_UserDataEx_getDoubleForKey 5  ret:{}",ret);
                
                tolua_pushnumber(tolua_S, (lua_Number)ret);
                //AXLOGD("lua_ax_base_UserDataEx_getDoubleForKey 6  end");
                return 1;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:getDoubleForKey", argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
        tolua_lerror:
            tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_getDoubleForKey'.", &tolua_err);
    #endif
        return 0;
}

int lua_ax_base_UserDataEx_setStringForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_setStringForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_setStringForKey argc:{}",argc);
        do
        {
            if (argc == 2)
            {
                std::string arg0;
                std::string arg1;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:setStringForKey");
                ok &= luaval_to_std_string(tolua_S, 3, &arg1, "UserDataEx:setStringForKey");

                if (!ok)
                {
                    break;
                }
                cobj->setStringForKey(arg0,arg1);
                return 0;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:setStringForKey", argc, 2);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_setStringForKey'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_getStringForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_getStringForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_getStringForKey argc:{}",argc);
        do
        {
            if (argc == 1)
            {
                std::string arg0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:getStringForKey");
                if (!ok)
                {
                    break;
                }
                std::string ret = cobj->getStringForKey(arg0);
                tolua_pushstring(tolua_S, ret.c_str());
                return 1;
            }
        } while (0);
        do
        {
            if (argc == 2)
            {
                std::string arg0;
                std::string arg1;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:getStringForKey");
                ok &= luaval_to_std_string(tolua_S, 3, &arg1, "UserDataEx:getStringForKey");
                if (!ok)
                {
                    break;
                }
                std::string ret = cobj->getStringForKey(arg0,arg1);
                tolua_pushstring(tolua_S, ret.c_str());
                return 1;
            }
        } while (0);

        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:getStringForKey", argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_getStringForKey'.", &tolua_err);
    #endif
    return 0;
}
int lua_ax_base_UserDataEx_setBoolForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_setBoolForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_setBoolForKey argc:{}",argc);
        do
        {
            if (argc == 2)
            {
                std::string arg0;
                bool arg1 = false;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:setBoolForKey");
                ok &= luaval_to_boolean(tolua_S, 3, &arg1, "UserDataEx:setBoolForKey");

                if (!ok)
                {
                    break;
                }
                cobj->setBoolForKey(arg0, arg1);
                return 0;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:setBoolForKey", argc, 2);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_setBoolForKey'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_getBoolForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
            if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_getBoolForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_getBoolForKey argc:{}",argc);
        do
        {
            if (argc == 1)
            {
                std::string arg0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "ax.UserDataEx:getBoolForKey");
                if (!ok)
                {
                    break;
                }
                bool ret = cobj->getBoolForKey(arg0);
                tolua_pushboolean(tolua_S, ret);
                return 1;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "ax.UserDataEx:getBoolForKey", argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_getBoolForKey'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_setIntegerForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_setIntegerForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_setIntegerForKey argc:{}",argc);
        do
        {
            if (argc == 2)
            {
                std::string arg0;
                long long arg1 = 0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:setIntegerForKey");
                ok &= luaval_to_long_long(tolua_S, 3, &arg1, "UserDataEx:setIntegerForKey");

                if (!ok)
                {
                    break;
                }
                cobj->setInt64ForKey(arg0, arg1);
                return 0;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:setIntegerForKey", argc, 2);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_setIntegerForKey'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_getIntegerForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_getIntegerForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_getIntegerForKey argc:{}",argc);
        do
        {
            if (argc == 1)
            {
                std::string arg0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:getIntegerForKey");
                if (!ok)
                {
                    break;
                }
                int64_t ret = cobj->getInt64ForKey(arg0);
                tolua_pushnumber(tolua_S, (lua_Number)ret);
                return 1;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:getIntegerForKey", argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_getIntegerForKey'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_setUnsignedForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_setUnsignedForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_setUnsignedForKey argc:{}",argc);
        do
        {
            if (argc == 2)
            {
                std::string arg0;
                long long arg1 = 0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:setUnsignedForKey");
                ok &= luaval_to_long_long(tolua_S, 3, &arg1, "UserDataEx:setUnsignedForKey");

                if (!ok)
                {
                    break;
                }
                cobj->setUnsignedInt64ForKey(arg0, arg1);
                return 0;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:setUnsignedForKey", argc, 2);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_setUnsignedForKey'.", &tolua_err);
    #endif
    return 0;
}
int lua_ax_base_UserDataEx_getUnsignedForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_getIntegerForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_getUnsignedForKey argc:{}",argc);
        do
        {
            if (argc == 1)
            {
                std::string arg0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:getUnsignedForKey");
                if (!ok)
                {
                    break;
                }
                uint64_t ret = cobj->getUnsignedInt64ForKey(arg0);
                tolua_pushnumber(tolua_S, (lua_Number)ret);
                return 1;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:getUnsignedForKey", argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_getUnsignedForKey'.", &tolua_err);
    #endif
    return 0;
}


int lua_ax_base_UserDataEx_setByteForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_setByteForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_setByteForKey argc:{}",argc);
        do
        {
            if (argc == 2)
            {
                std::string arg0;
                long long arg1 = 0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:setByteForKey");
                ok &= luaval_to_long_long(tolua_S, 3, &arg1, "UserDataEx:setByteForKey");

                if (!ok)
                {
                    break;
                }
                cobj->setByteForKey(arg0,unsigned char(arg1));
                return 0;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:setByteForKey", argc, 2);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_setByteForKey'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_getByteForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_getByteForKey'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_getByteForKey argc:{}",argc);
        do
        {
            if (argc == 1)
            {
                std::string arg0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:getByteForKey");
                if (!ok)
                {
                    break;
                }
                unsigned char ret = cobj->getByteForKey(arg0);
                tolua_pushnumber(tolua_S, (lua_Number)ret);
                return 1;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:getByteForKey", argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_getByteForKey'.", &tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_new(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;

    
    //  AXLOGD("lua_ax_base_UserDataEx_new  Check tolua_isusertable argc:{}",tolua_isusertable(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err));
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        // tolua_Error tolua_err;
        if (!tolua_isusertable(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
    argc = lua_gettop(tolua_S) - 1;
    AXLOGD("lua_ax_base_UserDataEx_new  Check argc:{}",argc);
    if (argc == 1)
    {
        std::string skey= tolua_tostring(tolua_S, 2, "");
        AXLOGD("lua_ax_base_UserDataEx_new  tostring  skey:{}",skey);
        if (skey.empty())
        {
            tolua_pushstring(tolua_S, "无效的参数在函数 'lua_ax_base_UserDataEx_new': 字符串参数为空");
            tolua_error(tolua_S, nullptr, nullptr);
            return 0;
        }

        UserDataEx* tolua_ret = (UserDataEx*)UserDataEx::GetUserDataEx(skey);
        AXLOGD("lua_ax_base_UserDataEx_new  skey:{} ret:tolua_ret:{}",skey,FMT_TOPOINT(tolua_ret));
        tolua_pushusertype(tolua_S, (void*)tolua_ret, "ax.UserDataEx");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "UserDataEx:new", argc, 1);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserDataEx_new'.", &tolua_err);
        return 0;
    #endif
}

int lua_ax_base_UserDataEx_destroyInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertable(tolua_S,1,"ax.UserDataEx",0,&tolua_err)) goto tolua_lerror;
    #endif

        argc = lua_gettop(tolua_S) - 1;

        if (argc == 1)
        {
            std::string skey;
            ok &= luaval_to_std_string(tolua_S, 2, &skey, "skey");

            if (!ok)
            {
                tolua_error(tolua_S,"invalid arguments in function 'lua_ax_base_UserDataEx_destroyInstance'", nullptr);
                return 0;
            }
            
            UserDataEx::DeleteUserDataEx(skey);
            lua_settop(tolua_S, 1);
            return 1;
        }
        luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "UserDataEx:destroyInstance",argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S,"#ferror in function 'lua_ax_base_UserDataEx_destroyInstance'.",&tolua_err);
    #endif
    return 0;
}

int lua_ax_base_UserDataEx_deleteForKey(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_delete'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_delete argc:{}",argc);
        do
        {
            if (argc == 1)
            {
                std::string arg0;
                ok &= luaval_to_std_string(tolua_S, 2, &arg0, "UserDataEx:deleteForKey");
                if (!ok)
                {
                    break;
                }
                bool ret = cobj->deleteForKey(arg0);
                tolua_pushboolean(tolua_S, ret);
                return 1;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:deleteForKey", argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_getBoolForKey'.", &tolua_err);
    #endif
    return 0;
}
int lua_ax_base_UserDataEx_setStorageName(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertable(tolua_S,1,"ax.UserDataEx",0,&tolua_err)) goto tolua_lerror;
    #endif

        argc = lua_gettop(tolua_S) - 1;

        if (argc == 1)
        {
            std::string skey;
            ok &= luaval_to_std_string(tolua_S, 2, &skey, "skey");

            if (!ok)
            {
                tolua_error(tolua_S,"invalid arguments in function 'lua_ax_base_UserDataEx_setStorageName'", nullptr);
                return 0;
            }
            
            UserDataEx::setStorageName(skey);
            lua_settop(tolua_S, 1);
            return 1;
        }
        luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "UserDataEx:setStorageName",argc, 1);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S,"#ferror in function 'lua_ax_base_UserDataEx_setStorageName'.",&tolua_err);
    #endif
    return 0;
}
int lua_ax_base_UserDataEx_getStorageName(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
    argc = lua_gettop(tolua_S) - 1;
    //AXLOGD("lua_ax_base_UserDataEx_getStorageName argc:{}",argc);
    do
    {
        if (argc == 0)
        {
            std::string ret(UserDataEx::getStorageName());
            tolua_pushstring(tolua_S, ret.c_str());
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:getStorageName", argc, 1);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_getStorageName'.", &tolua_err);
    #endif
    return 0;
}


int lua_ax_base_UserDataEx_setEncryptEnabled(lua_State* tolua_S)
{
    int argc         = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok          = true;
    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_setEncryptEnabled'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;
        //AXLOGD("lua_ax_base_UserDataEx_setEncryptEnabled argc:{}",argc);
        do
        {
            if (argc >1)
            {
                bool enabled;
                ok &= luaval_to_boolean(tolua_S, 2, &enabled, "UserDataEx:setEncryptEnabled");
                if (!ok)
                {
                    break;                          //如果参数未 正确 处理完成  退出
                }
                if ((enabled) && (argc ==3)) {
                    std::string arg1;
                    std::string arg2;
                    ok &= luaval_to_std_string(tolua_S, 3, &arg1, "UserDataEx:setEncryptEnabled");
                    ok &= luaval_to_std_string(tolua_S, 4, &arg2, "UserDataEx:setEncryptEnabled");
                    cobj->setEncryptEnabled(enabled, arg1, arg2);   //存在 
                }else
                {
                    cobj->setEncryptEnabled(enabled,"","");//取消 加密模式 其他参数 设置为空 不需要读取和扫描
                }
                return 0;
            }
        } while (0);
        ok = true;
        luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:setEncryptEnabled", argc, 3);
        return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_setEncryptEnabled'.", &tolua_err);
    #endif
    return 0;
}
// lua_ax_base_UserDataEx_setEncryptEnabled

int lua_ax_base_UserDataEx_clearAll(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertable(tolua_S,1,"ax.UserDataEx",0,&tolua_err)) goto tolua_lerror;
    #endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        // Since clearAll doesn't take any parameters, we can directly call it.
        UserDataEx::clearAll();

        // Optionally you can set the top of the stack back to where it was before the function call.
        lua_settop(tolua_S, 1);
        return 1; // Return value is not necessary since clearAll does not return anything.
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "UserDataEx:clearAll", argc, 0);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S,"#ferror in function 'lua_ax_base_UserDataEx_clearAll'.",&tolua_err);
    #endif
    return 0;
}
int lua_ax_base_UserDataEx_setAutoSave(lua_State* tolua_S)
{
    int argc = 0;
    ax::UserDataEx* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.UserDataEx", 0, &tolua_err))
            goto tolua_lerror;
    #endif
        cobj = (ax::UserDataEx*)tolua_tousertype(tolua_S, 1, 0);
    #if _AX_DEBUG >= 1
        if (!cobj)
        {
            tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_ax_base_UserDataEx_setAutoSave'", nullptr);
            return 0;
        }
    #endif
        argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        bool enabled;
        ok &= luaval_to_boolean(tolua_S, 2, &enabled, "UserDataEx:setAutoSave");

        if (!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_base_UserDataEx_setAutoSave'", nullptr);
            return 0;
        }

        cobj->setAutoSave(enabled);
        return 0;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "UserDataEx:setAutoSave", argc, 1);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_ax_base_UserDataEx_setAutoSave'.", &tolua_err);
    #endif
    return 0;
}


int lua_ax_base_TileMapManager_new(lua_State* tolua_S) {
    
    // AXLOGD("lua_ax_base_TileMapManager_new  进入");
    if (nullptr == tolua_S)
        return 0;

    int argc = 0;
    tolua_Error tolua_err;

    argc = lua_gettop(tolua_S) - 1;
    // AXLOGD("lua_ax_base_TileMapManager_new  进入  {}",argc);
    if (argc == 0) {
        // 使用 TileMapManager 的单例创建函数创建一个新的实例
        TileMapManager* tilemm = TileMapManager::create();
        // AXLOGD("lua_ax_base_TileMapManager_new tilemm:{}",fmt::ptr(tilemm));
        // 推入用户数据到 Lua 栈中，并设置其元表
        tolua_pushusertype(tolua_S, (void*)tilemm, "ax.TileMapManager");
        return 1;
    }

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "TileMapManager:new", argc, 1);
    return 0;

    #if _AX_DEBUG >= 1
        tolua_lerror:
            tolua_error(tolua_S, "#ferror in function 'lua_ax_base_TileMapManager_new'.", &tolua_err);
            return 0;
    #endif
}
// int lua_ax_base_TileMapManager_destroy(lua_State* tolua_S) {
//     int argc = 0;
//     bool ok = true;

//     #if _AX_DEBUG >= 1
//         tolua_Error tolua_err;
//     #endif

//     #if _AX_DEBUG >= 1
//         if (!tolua_isusertable(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err)) goto tolua_lerror;
//     #endif

//     argc = lua_gettop(tolua_S) - 1;

//     if (argc == 1) {
//         std::string skey;
//         ok &= luaval_to_std_string(tolua_S, 2, &skey, "skey");

//         if (!ok) {
//             tolua_error(tolua_S, "invalid arguments in function 'lua_ax_base_TileMapManager_destroy'", nullptr);
//             return 0;
//         }

//         // 这里假设 TileMapManager 有一个静态方法 destroyInstance 来销毁单例
//         TileMapManager::destroy();
//         lua_settop(tolua_S, 1);
//         return 1;
//     }
//     luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n", "TileMapManager:destroyInstance", argc, 1);
//     return 0;

//     #if _AX_DEBUG >= 1
//     tolua_lerror:
//         tolua_error(tolua_S, "#ferror in function 'lua_ax_base_TileMapManager_destroy'.", &tolua_err);
//     #endif
//     return 0;
// }

int lua_ax_base_TileMapManager_destroy(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_destroy'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0) {
        // uint16_t ret = cobj->destroy(x, y);
        // tolua_pushnumber(tolua_S, (lua_Number)ret);
        TileMapManager::destroy(cobj);
        return 0;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:destroy", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_destroy'.", &tolua_err);
    #endif
    return 0;
}
int lua_TileMapManager_GetBkImgIdx(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetBkImgIdx'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetBkImgIdx");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetBkImgIdx");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetBkImgIdx'", nullptr);
            return 0;
        }

        uint16_t ret = cobj->GetBkImgIdx(x, y);
        // AXLOGD("TileMapManager_GetFrImgIdx 在:[{},{}] 得到 ret:{}",x,y,ret);
        tolua_pushnumber(tolua_S, (lua_Number)ret);

        lua_Number lua_ret = lua_tonumber(tolua_S, -1);
        // AXLOGD("Pushed to Lua stack: {}", lua_ret);

        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetBkImgIdx", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetBkImgIdx'.", &tolua_err);
    #endif
    return 0;
}
int lua_TileMapManager_GetMidImgIdx(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetBkImgIdx'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetMidImgIdx");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetMidImgIdx");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetMidImgIdx'", nullptr);
            return 0;
        }

        uint16_t ret = cobj->GetMidImgIdx(x, y);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetMidImgIdx", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetMidImgIdx'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetFrImgIdx(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetBkImgIdx'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetFrImgIdx");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetFrImgIdx");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetFrImgIdx'", nullptr);
            return 0;
        }

        uint16_t ret = cobj->GetFrImgIdx(x, y);
        // AXLOGD("TileMapManager_GetFrImgIdx 在:[{},{}] 得到 ret:{}",x,y,ret);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetFrImgIdx", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetFrImgIdx'.", &tolua_err);
    #endif
    return 0;
}


int lua_TileMapManager_GetDoorIndex(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetDoorIndex'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetDoorIndex");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetDoorIndex");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetDoorIndex'", nullptr);
            return 0;
        }

        uint8_t ret = cobj->GetDoorIndex(x, y);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetDoorIndex", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetDoorIndex'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetDoorOffset(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetDoorOffset'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetDoorOffset");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetDoorOffset");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetDoorOffset'", nullptr);
            return 0;
        }

        uint8_t ret = cobj->GetDoorOffset(x, y);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
    
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetDoorOffset", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetDoorOffset'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_isObstacle(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_isObstacle'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:isObstacle");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:isObstacle");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_isObstacle'", nullptr);
            return 0;
        }

        bool ret = cobj->isObstacle(x, y); // 假设 isObstacle 是非静态成员函数
        tolua_pushboolean(tolua_S, ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:isObstacle", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_isObstacle'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetAniFrame(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetAniFrame'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetAniFrame");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetAniFrame");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetAniFrame'", nullptr);
            return 0;
        }

        uint8_t ret = cobj->GetAniFrame(x, y); // 假设 GetAniFrame 是非静态成员函数
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetAniFrame", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetAniFrame'.", &tolua_err);
    #endif
    return 0;
}
int lua_TileMapManager_GetAniTick(lua_State* tolua_S) {
    // 检查是否为用户类型
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1; // 获取参数数量

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetAniTick'", nullptr);
        return 0;
    }

    // 检查参数数量并提取参数
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetAniTick");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetAniTick");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetAniTick'", nullptr);
            return 0;
        }

        // 调用 TileMapManager 的 GetAniTick 方法
        uint8_t ret = cobj->GetAniTick(x, y);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetAniTick", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetAniTick'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetTilesArea(lua_State* tolua_S) {
    // 检查是否为用户类型
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetTilesArea'", nullptr);
        return 0;
    }

    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetTilesArea");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetTilesArea");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetTilesArea'", nullptr);
            return 0;
        }

        uint8_t ret = cobj->GetTilesArea(x, y);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetTilesArea", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetTilesArea'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetSmTilesArea(lua_State* tolua_S) {
    // 实现与之前函数类似，仅方法调用不同
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetSmTilesArea'", nullptr);
        return 0;
    }

    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetSmTilesArea");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetSmTilesArea");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetSmTilesArea'", nullptr);
            return 0;
        }

        uint8_t ret = cobj->GetSmTilesArea(x, y);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetSmTilesArea", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetSmTilesArea'.", &tolua_err);
    #endif
    return 0;
}
int lua_TileMapManager_GetObjArea(lua_State* tolua_S) {
    // 实现与之前函数类似，仅方法调用不同
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetObjArea'", nullptr);
        return 0;
    }

    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetObjArea");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetObjArea");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetObjArea'", nullptr);
            return 0;
        }

        uint8_t ret = cobj->GetObjArea(x, y);
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetObjArea", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetObjArea'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetMapDataRows(lua_State* tolua_S) {
    // 不需要 x, y 参数，因此 argc 应为 1
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_getMapDataRows'", nullptr);
        return 0;
    }

    if (argc == 0) {
        uint32_t ret = cobj->getHeight();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:getHeight", argc, 1);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_getMapDataRows'.", &tolua_err);
    #endif
    return 0;
}
int lua_TileMapManager_GetVersion(lua_State* tolua_S) {
    // 不需要 x, y 参数，因此 argc 应为 1
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetVersion'", nullptr);
        return 0;
    }

    if (argc == 1) {
        uint32_t ret = cobj->getVersion();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetVersion", argc, 1);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_getMapDataRows'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetMapDataCols(lua_State* tolua_S) {
    // 不需要 x, y 参数，因此 argc 应为 1
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetMapDataCols'", nullptr);
        return 0;
    }

    if (argc == 0) {
        uint32_t ret = cobj->getWidth();
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:getWidth", argc, 1);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetMapDataCols'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetLight(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetLight'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetLight");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetLight");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetLight'", nullptr);
            return 0;
        }

        uint8_t ret = cobj->GetLight(x, y); // 假设 GetLight 是非静态成员函数
        tolua_pushnumber(tolua_S, (lua_Number)ret);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetLight", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetLight'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_SetDoorOffset(lua_State* tolua_S) {
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_setDoorOffset'", nullptr);
        return 0;
    }

    if (argc == 3) {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t doorOffset = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:setDoorOffset");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:setDoorOffset");
        ok &= luaval_to_uint32(tolua_S, 4, &doorOffset, "TileMapManager:setDoorOffset");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_setDoorOffset'", nullptr);
            return 0;
        }

        cobj->setDoorOffset(x, y,static_cast<uint8_t>(doorOffset));
        lua_settop(tolua_S, 1);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:setDoorOffset", argc, 3);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_setDoorOffset'.", &tolua_err);
    #endif
    return 0;
}


int lua_TileMapManager_SetObstacle(lua_State* tolua_S) {
    TileMapManager* cobj = nullptr;
    bool ok = true;
    int argc = lua_gettop(tolua_S) - 1;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_setObstacle'", nullptr);
        return 0;
    }

    if (argc == 3) {
        uint32_t x = 0;
        uint32_t y = 0;
        bool obstacle = false;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:setObstacle");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:setObstacle");
        ok &= luaval_to_boolean(tolua_S, 4, &obstacle, "TileMapManager:setObstacle");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_setObstacle'", nullptr);
            return 0;
        }

        cobj->setObstacle(x, y, obstacle);
        lua_settop(tolua_S, 1);
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:setObstacle", argc, 3);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_setObstacle'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_LoadData(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_LoadData'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1) {
        std::string filename;
        ok &= luaval_to_std_string(tolua_S, 2, &filename, "ax.TileMapManager:loadMapFile");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_LoadData'", nullptr);
            return 0;
        }

        bool ret = cobj->loadMapFile(filename); // 调用 TileMapManager 的 loadMapFile 方法
        tolua_pushboolean(tolua_S, ret); // 将返回值推入 Lua 栈
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "ax.TileMapManager:loadMapFile", argc, 1);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_LoadData'.", &tolua_err);
    #endif
    return 0;
}

int lua_TileMapManager_GetTileInfoEx(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    // AXLOGD("基于 GetTileInfoEx 得到 cobj:{}",fmt::ptr(cobj));
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetTileInfoEx'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetTileInfoEx");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetTileInfoEx");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetTileInfoEx'", nullptr);
            return 0;
        }

        TGMapInfo* tileInfo = cobj->GetTileInfo(x, y); // 获取 TGMapInfo 对象
        // AXLOGD("基于 GetTileInfoEx 得到x{}  y{}   TGMapInfo* tileInfo:{}",x,y,fmt::ptr(tileInfo));
        if (tileInfo == nullptr) {
            lua_pushnil(tolua_S); // 如果没有有效的 TGMapInfo 对象，推入 nil
            return 1;
        }

        // 将 TGMapInfo 的属性依次压入 Lua 栈中
        // AXLOGD("基于 GetTileInfoEx tileInfo:{} {} {} {} {} {} {} ",
        //                 tileInfo->getwBkImg(),
        //                 tileInfo->getwMidImg(),
        //                 tileInfo->getwFrImg(),
        //                 tileInfo->getDoorIndex(),
        //                 tileInfo->getDoorOffset(),
        //                 tileInfo->getAniFrame(),
        //                 tileInfo->getAniTick());

        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getwBkImg());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getwMidImg());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getwFrImg());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getAniImg());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getDoorIndex());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getDoorOffset());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getAniFrame());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getAniTick());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getLight());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getUnitBkImg());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getUnitMidImg());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getUnitFrImg());
        // tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getUnitAniImg());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getAniTilesFrame());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getAniTilesTick());
        tolua_pushnumber(tolua_S, (lua_Number)tileInfo->getAniType());
        return 15; // 返回压入栈中的值的数量
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetTileInfoEx", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
        tolua_lerror:
            tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetTileInfoEx'.", &tolua_err);
    #endif
    return 0;
}


int lua_TileMapManager_GetTileInfo(lua_State* tolua_S) {
    int argc = 0;
    TileMapManager* cobj = nullptr;
    bool ok = true;

    #if _AX_DEBUG >= 1
        tolua_Error tolua_err;
    #endif

    #if _AX_DEBUG >= 1
        if (!tolua_isusertype(tolua_S, 1, "ax.TileMapManager", 0, &tolua_err))
            goto tolua_lerror;
    #endif

    cobj = (TileMapManager*)tolua_tousertype(tolua_S, 1, 0);
    if (!cobj) {
        tolua_error(tolua_S, "无效的 'cobj' 在函数 'lua_TileMapManager_GetTileInfo'", nullptr);
        return 0;
    }

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 2) {
        uint32_t x = 0;
        uint32_t y = 0;
        ok &= luaval_to_uint32(tolua_S, 2, &x, "TileMapManager:GetTileInfo");
        ok &= luaval_to_uint32(tolua_S, 3, &y, "TileMapManager:GetTileInfo");

        if (!ok) {
            tolua_error(tolua_S, "无效的参数在函数 'lua_TileMapManager_GetTileInfo'", nullptr);
            return 0;
        }

        // 调用 TileMapManager 的 GetTileInfo 方法
        TGMapInfo* tileInfo = cobj->GetTileInfo(x, y);
        if (tileInfo == nullptr) {
            lua_pushnil(tolua_S); // 如果没有有效的 TGMapInfo 对象，推入 nil
            return 1;
        }

        // 推入 TGMapInfo 指针到 Lua 栈中，并设置其元表
        tolua_pushusertype(tolua_S, (void*)tileInfo, "ax.TGMapInfo");
        return 1;
    }

    luaL_error(tolua_S, "%s 参数数量错误: %d, 应该期望 %d\n", "TileMapManager:GetTileInfo", argc, 2);
    return 0;

    #if _AX_DEBUG >= 1
    tolua_lerror:
        tolua_error(tolua_S, "#ferror 在函数 'lua_TileMapManager_GetTileInfo'.", &tolua_err);
    #endif
    return 0;
}

int register_all_axlua_bindings_TileMapManager(lua_State* tolua_S)
{
    tolua_usertype(tolua_S, "ax.TileMapManager");
    tolua_cclass(tolua_S, "TileMapManager", "ax.TileMapManager", "ax.Object", nullptr);
    tolua_beginmodule(tolua_S, "TileMapManager");
    //------------------------------------------------------------------------------------
    //这些都是 类方法 //获得 一个 指定键名 的 字典对象
    tolua_function(tolua_S, "create", lua_ax_base_TileMapManager_new);                      //创建 实例
    // AXLOGD("register_all_axlua_bindings_TileMapManager");    
    tolua_function(tolua_S,"destroy", lua_ax_base_TileMapManager_destroy);                  //释放 实例
    // //------------------------------------------------------------------------------------
    // //下面都是 实例方法
    tolua_function(tolua_S, "GetBkImgIdx", lua_TileMapManager_GetBkImgIdx);
    tolua_function(tolua_S, "GetMidImgIdx", lua_TileMapManager_GetMidImgIdx);
    tolua_function(tolua_S, "GetFrImgIdx", lua_TileMapManager_GetFrImgIdx);
    tolua_function(tolua_S, "GetDoorIndex", lua_TileMapManager_GetDoorIndex);
    tolua_function(tolua_S, "GetDoorOffset", lua_TileMapManager_GetDoorOffset);
    tolua_function(tolua_S, "isObstacle", lua_TileMapManager_isObstacle);
    tolua_function(tolua_S, "GetAniFrame", lua_TileMapManager_GetAniFrame);
    tolua_function(tolua_S, "GetAniTick", lua_TileMapManager_GetAniTick);
    tolua_function(tolua_S, "GetTilesArea", lua_TileMapManager_GetTilesArea);
    tolua_function(tolua_S, "GetSmTilesArea",lua_TileMapManager_GetSmTilesArea);
    tolua_function(tolua_S, "GetObjArea", lua_TileMapManager_GetObjArea);
    tolua_function(tolua_S, "GetMapDataRows", lua_TileMapManager_GetMapDataRows);
    tolua_function(tolua_S, "GetMapDataCols", lua_TileMapManager_GetMapDataCols);
    tolua_function(tolua_S, "GetLight", lua_TileMapManager_GetLight);
    tolua_function(tolua_S, "SetDoorOffset", lua_TileMapManager_SetDoorOffset);
    tolua_function(tolua_S, "SetObstacle", lua_TileMapManager_SetObstacle);
    tolua_function(tolua_S, "LoadData", lua_TileMapManager_LoadData);
    tolua_function(tolua_S, "GetTileInfoEx", lua_TileMapManager_GetTileInfoEx);
    tolua_function(tolua_S, "GetTileInfo", lua_TileMapManager_GetTileInfo);
    tolua_function(tolua_S, "GetVersion", lua_TileMapManager_GetVersion);
    // //AXLOGI(" bindings UserDATAEx End");
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::TileMapManager).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.TileMapManager";
    g_typeCast[typeName] = "ax.TileMapManager";
    // AXLOGD("register_all_axlua_bindings_TileMapManager");    
    return 1;
    // auto typeName = typeid(ax::UserDataEx).name(); // rtti is literal storage
    // g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.UserDataEx";
    // g_typeCast[typeName] = "ax.UserDataEx";

}
// Registration function to bind UserDataEx class to Lua
int register_all_axlua_bindings_UserDATAEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S, "ax.UserDataEx");
    //AXLOGI(" bindings UserDATAEx Start ax.UserDataEx");
    tolua_cclass(tolua_S, "UserDataEx", "ax.UserDataEx", "ax.Object", nullptr);

    tolua_beginmodule(tolua_S, "UserDataEx");

    // Bind constructor and destructor
    //------------------------------------------------------------------------------------
    //这些都是 类方法 
    //获得 一个 指定键名 的 字典对象
    tolua_function(tolua_S, "new", lua_ax_base_UserDataEx_new);
    //AXLOGI(" bindings UserDATAEx Add New");
    //释放一个 指定键名 的 字典对象 该对象 的数据会立即持久化
    tolua_function(tolua_S,"destroyInstance", lua_ax_base_UserDataEx_destroyInstance);
    tolua_function(tolua_S,"setStorageName", lua_ax_base_UserDataEx_setStorageName);
    tolua_function(tolua_S,"getStorageName", lua_ax_base_UserDataEx_getStorageName);
    tolua_function(tolua_S,"Cleanup", lua_ax_base_UserDataEx_clearAll);
    tolua_function(tolua_S,"setVersionPath", lua_ax_base_UserDataEx_setStorageName);
   
    //三个 静态方法 
	//	setStorageName	设置整体存储名称	todo
	//	getStorageName	获得当前存储名称	todo
    //------------------------------------------------------------------------------------
    //下面都是 实例方法
    //  setEncryptEnabled()					设置加密
    tolua_function(tolua_S, "setDoubleForKey", lua_ax_base_UserDataEx_setDoubleForKey);
    tolua_function(tolua_S, "setStringForKey", lua_ax_base_UserDataEx_setStringForKey);
    tolua_function(tolua_S, "setBoolForKey", lua_ax_base_UserDataEx_setBoolForKey);
    tolua_function(tolua_S, "setIntegerForKey", lua_ax_base_UserDataEx_setIntegerForKey);
    tolua_function(tolua_S, "setUnsignedForKey", lua_ax_base_UserDataEx_setUnsignedForKey);
    tolua_function(tolua_S, "setByteForKey", lua_ax_base_UserDataEx_setByteForKey);
    // // Bind get methods
    tolua_function(tolua_S, "getDoubleForKey", lua_ax_base_UserDataEx_getDoubleForKey);
    tolua_function(tolua_S, "getStringForKey", lua_ax_base_UserDataEx_getStringForKey);
    tolua_function(tolua_S, "getBoolForKey", lua_ax_base_UserDataEx_getBoolForKey);
    tolua_function(tolua_S, "getIntegerForKey", lua_ax_base_UserDataEx_getIntegerForKey);
    tolua_function(tolua_S, "getUnsignedForKey", lua_ax_base_UserDataEx_getUnsignedForKey);
    tolua_function(tolua_S, "getByteForKey", lua_ax_base_UserDataEx_getByteForKey);
    //删除 一个指定的 Key ，也就是 该键值 在一个字典内不存在。
    tolua_function(tolua_S, "deleteForkey", lua_ax_base_UserDataEx_deleteForKey);
    tolua_function(tolua_S, "setEncryptEnabled", lua_ax_base_UserDataEx_setEncryptEnabled);
    tolua_function(tolua_S, "writeMapDataToFile", lua_ax_base_UserDataEx_saveData);
    tolua_function(tolua_S, "saveData", lua_ax_base_UserDataEx_saveData); 
    tolua_function(tolua_S, "setAutoSave", lua_ax_base_UserDataEx_setAutoSave); 
    //AXLOGI(" bindings UserDATAEx End");
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::UserDataEx).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.UserDataEx";
    g_typeCast[typeName] = "ax.UserDataEx";
    return 1;
}
