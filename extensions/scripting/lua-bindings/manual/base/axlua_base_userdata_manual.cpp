#include "axlua_base_userdata_manual.h"

#include "lua-bindings/manual/tolua_fix.h"

using namespace ax;

// UserData.create(storageName)
int lua_ax_base_UserData_create(lua_State* tolua_S) {
  AXLOGD("lua_ax_base_UserData_create");
  int argc = 0;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertable(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc == 1) {
    std::string_view arg0;
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    if (!ok) {
      tolua_error(tolua_S,
                  "invalid arguments in function 'lua_ax_base_UserData_create'",
                  nullptr);
      return 0;
    }
    auto ret = UserData::create(arg0);
    AXLOGD("lua_ax_base_UserData_create ok");
    object_to_luaval<UserData>(tolua_S, "UserData", (UserData*)ret);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:create", argc, 1);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_create'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:setString(key, value)
int lua_ax_base_UserData_setString(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_setString'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc == 2) {
    std::string_view arg0;
    std::string arg1;
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    ok &= luaval_to_std_string(tolua_S, 3, &arg1);
    if (!ok) {
      tolua_error(
          tolua_S,
          "invalid arguments in function 'lua_ax_base_UserData_setString'",
          nullptr);
      return 0;
    }
    cobj->setStringForKey(arg0, arg1);
    lua_settop(tolua_S, 1);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:setString", argc, 2);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_setString'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:getString(key, defaultValue)
int lua_ax_base_UserData_getString(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_getString'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc >= 1) {
    std::string_view arg0;
    std::string arg1 = "";
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    if (argc >= 2) {
      ok &= luaval_to_std_string(tolua_S, 3, &arg1);
    }
    if (!ok) {
      tolua_error(
          tolua_S,
          "invalid arguments in function 'lua_ax_base_UserData_getString'",
          nullptr);
      return 0;
    }
    auto ret = cobj->getStringForKey(arg0, arg1);
    tolua_pushcppstring(tolua_S, ret);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:getString", argc, 1);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_getString'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:setDouble(key, value)
int lua_ax_base_UserData_setDouble(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_setDouble'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc == 2) {
    std::string_view arg0;
    double arg1;
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    ok &= luaval_to_number(tolua_S, 3, &arg1);
    if (!ok) {
      tolua_error(
          tolua_S,
          "invalid arguments in function 'lua_ax_base_UserData_setDouble'",
          nullptr);
      return 0;
    }
    cobj->setDoubleForKey(arg0, arg1);
    lua_settop(tolua_S, 1);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:setDouble", argc, 2);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_setDouble'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:getDouble(key, defaultValue)
int lua_ax_base_UserData_getDouble(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_getDouble'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc >= 1) {
    std::string_view arg0;
    double arg1 = 0.0;
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    if (argc >= 2) {
      ok &= luaval_to_number(tolua_S, 3, &arg1);
    }
    if (!ok) {
      tolua_error(
          tolua_S,
          "invalid arguments in function 'lua_ax_base_UserData_getDouble'",
          nullptr);
      return 0;
    }
    auto ret = cobj->getDoubleForKey(arg0, arg1);
    tolua_pushnumber(tolua_S, (lua_Number)ret);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:getDouble", argc, 1);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_getDouble'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:setBool(key, value)
int lua_ax_base_UserData_setBool(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_setBool'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc == 2) {
    std::string_view arg0;
    bool arg1;
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    ok &= luaval_to_boolean(tolua_S, 3, &arg1);
    if (!ok) {
      tolua_error(
          tolua_S,
          "invalid arguments in function 'lua_ax_base_UserData_setBool'",
          nullptr);
      return 0;
    }
    cobj->setBoolForKey(arg0, arg1);
    lua_settop(tolua_S, 1);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:setBool", argc, 2);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_setBool'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:getBool(key, defaultValue)
int lua_ax_base_UserData_getBool(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_getBool'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc >= 1) {
    std::string_view arg0;
    bool arg1 = false;
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    if (argc >= 2) {
      ok &= luaval_to_boolean(tolua_S, 3, &arg1);
    }
    if (!ok) {
      tolua_error(
          tolua_S,
          "invalid arguments in function 'lua_ax_base_UserData_getBool'",
          nullptr);
      return 0;
    }
    auto ret = cobj->getBoolForKey(arg0, arg1);
    tolua_pushboolean(tolua_S, ret);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:getBool", argc, 1);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_getBool'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:setInteger(key, value)
int lua_ax_base_UserData_setInteger(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_setInteger'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc == 2) {
    std::string_view arg0;
    int arg1;
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    ok &= luaval_to_int32(tolua_S, 3, &arg1);
    if (!ok) {
      tolua_error(
          tolua_S,
          "invalid arguments in function 'lua_ax_base_UserData_setInteger'",
          nullptr);
      return 0;
    }
    cobj->setIntegerForKey(arg0, arg1);
    lua_settop(tolua_S, 1);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:setInteger", argc, 2);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_setInteger'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:getInteger(key, defaultValue)
int lua_ax_base_UserData_getInteger(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_getInteger'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc >= 1) {
    std::string_view arg0;
    int arg1 = 0;
    ok &= luaval_to_std_string_view(tolua_S, 2, &arg0);
    if (argc >= 2) {
      ok &= luaval_to_int32(tolua_S, 3, &arg1);
    }
    if (!ok) {
      tolua_error(
          tolua_S,
          "invalid arguments in function 'lua_ax_base_UserData_getInteger'",
          nullptr);
      return 0;
    }
    auto ret = cobj->getIntegerForKey(arg0, arg1);
    tolua_pushnumber(tolua_S, (lua_Number)ret);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:getInteger", argc, 1);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_getInteger'.",
              &tolua_err);
#endif
  return 0;
}

// UserData:write()
int lua_ax_base_UserData_write(lua_State* tolua_S) {
  int argc = 0;
  UserData* cobj = nullptr;
  bool ok = true;

#if _AX_DEBUG >= 1
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S, 1, "UserData", 0, &tolua_err))
    goto tolua_lerror;
#endif

  cobj = (UserData*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
  if (!cobj) {
    tolua_error(tolua_S,
                "invalid 'cobj' in function 'lua_ax_base_UserData_write'",
                nullptr);
    return 0;
  }
#endif

  argc = lua_gettop(tolua_S) - 1;
  if (argc == 0) {
    cobj->writeMapData();
    lua_settop(tolua_S, 1);
    return 1;
  }
  luaL_error(tolua_S,
             "%s has wrong number of arguments: %d, was expecting %d\n",
             "UserData:write", argc, 0);
  return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
  tolua_error(tolua_S, "#ferror in function 'lua_ax_base_UserData_write'.",
              &tolua_err);
#endif
  return 0;
}

int lua_ax_base_UserData_finalize(lua_State* tolua_S) {
  UserData* self = (UserData*)tolua_tousertype(tolua_S, 1, 0);
  AXLOGV("luabindings: finalizing LUA object (UserData)");
  if (self) {
    delete self;
  }
  return 0;
}

int lua_register_ax_base_UserData(lua_State* tolua_S) {
  AXLOGD("lua_register_ax_base_UserData");
  // tolua_cclass(tolua_S, "ScriptHandlerMgr", "ScriptHandlerMgr", "", NULL);
  tolua_cclass(tolua_S, "UserData", "UserData", "",
               NULL);  //               lua_ax_base_UserData_finalize);
  tolua_beginmodule(tolua_S, "UserData");
  tolua_function(tolua_S, "new", lua_ax_base_UserData_create);
  tolua_function(tolua_S, "setStringForKey", lua_ax_base_UserData_setString);
  tolua_function(tolua_S, "getStringForKey", lua_ax_base_UserData_getString);
  tolua_function(tolua_S, "setDoubleForKey", lua_ax_base_UserData_setDouble);
  tolua_function(tolua_S, "getDoubleForKey", lua_ax_base_UserData_getDouble);
  tolua_function(tolua_S, "setBoolForKey", lua_ax_base_UserData_setBool);
  tolua_function(tolua_S, "getBoolForKey", lua_ax_base_UserData_getBool);
  tolua_function(tolua_S, "setIntegerForKey", lua_ax_base_UserData_setInteger);
  tolua_function(tolua_S, "getIntegerForKey", lua_ax_base_UserData_getInteger);
  tolua_function(tolua_S, "writeMapDataToFile", lua_ax_base_UserData_write);
  tolua_endmodule(tolua_S);
  auto typeName = typeid(UserData).name();
  g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "UserData";
  g_typeCast[typeName] = "UserData";
  return 1;
}

int register_userdata_module(lua_State* tolua_S) {
  tolua_open(tolua_S);
  tolua_usertype(tolua_S, "UserData");
  tolua_module(tolua_S, NULL, 0);
  tolua_beginmodule(tolua_S, NULL);
  lua_register_ax_base_UserData(tolua_S);
  tolua_endmodule(tolua_S);
  return 1;
}