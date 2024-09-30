/****************************************************************************
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

#include "lua-bindings/manual/tolua_fix.h"
#include "base/Object.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include <stdlib.h>
#include "base/Logging.h"
#include "lua_stackdump.h"

using namespace ax;

static int s_function_ref_id = 0;
// luaSD_printf _Logprintf = logprint;
// #ifndef luaSD_PRINT
// #include <stdio.h>
// #define luaSD_PRINT logprint
// #endif

#include <iostream>
#include <queue>
#include <string>
#include <mutex>

class StringQueue {
private:
    std::queue<std::string> queue;
    mutable std::mutex mutex;

public:
    void lock() {
        mutex.lock();
    }

    // 解锁
    void unlock() {
        mutex.unlock();
    }

    // 入列函数
    void enqueue(const std::string& item) {
        queue.push(item);
    }

    // 出列函数
    bool dequeue(std::string& item) {
        if (queue.empty()) {
            // 队列空时返回 false
            return false;
        }
        item = queue.front();
        queue.pop();
        return true;
    }

    // 检查队列是否为空
    bool isEmpty() const {
        return queue.empty();
    }

    // 获取队列大小
    size_t size() const {
        return queue.size();
    }
};


static StringQueue sq;



TOLUA_API void toluafix_open(lua_State* L)
{
    lua_pushstring(L, TOLUA_REFID_PTR_MAPPING);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, TOLUA_REFID_TYPE_MAPPING);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

TOLUA_API int toluafix_pushusertype_object(lua_State* L, int refid, int* p_refid, void* ptr, const char* type)
{
    if (ptr == NULL || p_refid == NULL)
    {
        lua_pushnil(L);
        return -1;
    }

    Object* vPtr         = static_cast<Object*>(ptr);
    const char* vType = getLuaTypeName(vPtr, type);

    if (*p_refid == 0)
    {
        *p_refid = refid;

        lua_pushstring(L, TOLUA_REFID_PTR_MAPPING);
        lua_rawget(L, LUA_REGISTRYINDEX); /* stack: refid_ptr */
        lua_pushinteger(L, refid);        /* stack: refid_ptr refid */
        lua_pushlightuserdata(L, vPtr);   /* stack: refid_ptr refid ptr */

        lua_rawset(L, -3); /* refid_ptr[refid] = ptr, stack: refid_ptr */
        lua_pop(L, 1);     /* stack: - */

        lua_pushstring(L, TOLUA_REFID_TYPE_MAPPING);
        lua_rawget(L, LUA_REGISTRYINDEX); /* stack: refid_type */
        lua_pushinteger(L, refid);        /* stack: refid_type refid */
        lua_pushstring(L, vType);         /* stack: refid_type refid type */
        lua_rawset(L, -3);                /* refid_type[refid] = type, stack: refid_type */
        lua_pop(L, 1);                    /* stack: - */

        // AXLOGD("[LUA] push CCObject OK - refid: {}, ptr: {}, type: {}\n", *p_refid, (int)ptr, type);
    }

    tolua_pushusertype_and_addtoroot(L, vPtr, vType);

    return 0;
}

TOLUA_API int toluafix_remove_ccobject_by_refid(lua_State* L, int refid)
{
    void* ptr        = NULL;
    const char* type = NULL;
    void** ud        = NULL;
    if (refid == 0)
        return -1;

    // get ptr from tolua_refid_ptr_mapping
    lua_pushstring(L, TOLUA_REFID_PTR_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX); /* stack: refid_ptr */
    lua_pushinteger(L, refid);        /* stack: refid_ptr refid */
    lua_rawget(L, -2);                /* stack: refid_ptr ptr */
    ptr = lua_touserdata(L, -1);
    lua_pop(L, 1); /* stack: refid_ptr */
    if (ptr == NULL)
    {
        lua_pop(L, 1);
        // Lua stack has closed, C++ object not in Lua.
        // printf("[LUA ERROR] remove CCObject with NULL ptr, refid: %d\n", refid);
        return -2;
    }

    // remove ptr from tolua_refid_ptr_mapping
    lua_pushinteger(L, refid); /* stack: refid_ptr refid */
    lua_pushnil(L);            /* stack: refid_ptr refid nil */
    lua_rawset(L, -3);         /* delete refid_ptr[refid], stack: refid_ptr */
    lua_pop(L, 1);             /* stack: - */

    // get type from tolua_refid_type_mapping
    lua_pushstring(L, TOLUA_REFID_TYPE_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX); /* stack: refid_type */
    lua_pushinteger(L, refid);        /* stack: refid_type refid */
    lua_rawget(L, -2);                /* stack: refid_type type */
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 2);
        AXLOGD("[LUA ERROR] remove CCObject with NULL type, refid: {}, ptr: {}\n", refid, fmt::ptr(ptr));
        return -1;
    }

    type = lua_tostring(L, -1);
    lua_pop(L, 1); /* stack: refid_type */

    // remove type from tolua_refid_type_mapping
    lua_pushinteger(L, refid); /* stack: refid_type refid */
    lua_pushnil(L);            /* stack: refid_type refid nil */
    lua_rawset(L, -3);         /* delete refid_type[refid], stack: refid_type */
    lua_pop(L, 1);             /* stack: - */

    // get ubox
    luaL_getmetatable(L, type);      /* stack: mt */
    lua_pushstring(L, "tolua_ubox"); /* stack: mt key */
    lua_rawget(L, -2);               /* stack: mt ubox */
    if (lua_isnil(L, -1))
    {
        // use global ubox
        lua_pop(L, 1);                    /* stack: mt */
        lua_pushstring(L, "tolua_ubox");  /* stack: mt key */
        lua_rawget(L, LUA_REGISTRYINDEX); /* stack: mt ubox */
    };

    // cleanup root
    tolua_remove_value_from_root(L, ptr);

    lua_pushlightuserdata(L, ptr); /* stack: mt ubox ptr */
    lua_rawget(L, -2);             /* stack: mt ubox ud */
    if (lua_isnil(L, -1))
    {
        // Lua object has released (GC), C++ object not in ubox.
        // printf("[LUA ERROR] remove CCObject with NULL ubox, refid: %d, ptr: %x, type: %s\n", refid, (int)ptr, type);
        lua_pop(L, 3);
        return -3;
    }

    // cleanup peertable
    lua_pushvalue(L, LUA_REGISTRYINDEX);
    lua_setfenv(L, -2);

    ud = (void**)lua_touserdata(L, -1);
    lua_pop(L, 1); /* stack: mt ubox */
    if (ud == NULL)
    {
        AXLOGD("[LUA ERROR] remove CCObject with NULL userdata, refid: {}, ptr: {}, type: {}n", refid, fmt::ptr(ptr), type);
        lua_pop(L, 2);
        return -1;
    }

    // clean userdata
    *ud = NULL;

    lua_pushlightuserdata(L, ptr); /* stack: mt ubox ptr */
    lua_pushnil(L);                /* stack: mt ubox ptr nil */
    lua_rawset(L, -3);             /* ubox[ptr] = nil, stack: mt ubox */

    lua_pop(L, 2);
    // printf("[LUA] remove CCObject, refid: %d, ptr: %x, type: %s\n", refid, (int)ptr, type);
    return 0;
}

TOLUA_API int toluafix_ref_function(lua_State* L, int lo, int def)
{
    // function at lo
    if (!lua_isfunction(L, lo))
        return 0;

    s_function_ref_id++;

    lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX);      /* stack: fun ... refid_fun */
    lua_pushinteger(L, s_function_ref_id); /* stack: fun ... refid_fun refid */
    lua_pushvalue(L, lo);                  /* stack: fun ... refid_fun refid fun */

    lua_rawset(L, -3); /* refid_fun[refid] = fun, stack: fun ... refid_ptr */
    lua_pop(L, 1);     /* stack: fun ... */

    return s_function_ref_id;

    // lua_pushvalue(L, lo);                                           /* stack: ... func */
    // return luaL_ref(L, LUA_REGISTRYINDEX);
}

TOLUA_API void toluafix_get_function_by_refid(lua_State* L, int refid)
{
    lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX); /* stack: ... refid_fun */
    lua_pushinteger(L, refid);        /* stack: ... refid_fun refid */
    lua_rawget(L, -2);                /* stack: ... refid_fun fun */
    lua_remove(L, -2);                /* stack: ... fun */
}

TOLUA_API void toluafix_remove_function_by_refid(lua_State* L, int refid)
{
    lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX); /* stack: ... refid_fun */
    lua_pushinteger(L, refid);        /* stack: ... refid_fun refid */
    lua_pushnil(L);                   /* stack: ... refid_fun refid nil */
    lua_rawset(L, -3);                /* refid_fun[refid] = fun, stack: ... refid_ptr */
    lua_pop(L, 1);                    /* stack: ... */

    // luaL_unref(L, LUA_REGISTRYINDEX, refid);
}

// check lua value is function
TOLUA_API int toluafix_isfunction(lua_State* L, int lo, const char* type, int def, tolua_Error* err)
{
    if (lua_gettop(L) >= abs(lo) && lua_isfunction(L, lo))
    {
        return 1;
    }
    err->index = lo;
    err->array = 0;
    err->type  = "[not function]";
    return 0;
}

TOLUA_API int toluafix_totable(lua_State* L, int lo, int def)
{
    return lo;
}

TOLUA_API int toluafix_istable(lua_State* L, int lo, const char* type, int def, tolua_Error* err)
{
    return tolua_istable(L, lo, def, err);
}

TOLUA_API void toluafix_stack_dump(lua_State* L, const char* label)
{
    int i;
    int top = lua_gettop(L);
    AXLOGD("[SD] Total [{}] in lua stack: {}", top, label != 0 ? label : "");
    for (i = -1; i >= -top; i--)
    {
        int t = lua_type(L, i);
        switch (t)
        {
            case LUA_TSTRING:
                AXLOGD("[SD]  [{:2}] string {}", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                AXLOGD("[SD]  [{:2}] boolean {}", i, lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                AXLOGD("[SD]  [{:2}] number {}", i, lua_tonumber(L, i));
                break;
            case LUA_TNIL:
                AXLOGD("[SD]  [{:2}]  number nil",i);
                break;
            case LUA_TFUNCTION:
                AXLOGD("[SD]  [{:2}] function {}",i,lua_topointer(L, i));
                break;
            // #define LUA_TLIGHTUSERDATA	2
            // #define LUA_TTABLE		5
            // #define LUA_TUSERDATA		7
            // #define LUA_TTHREAD		8
            default:
                AXLOGD("[SD]  [{:2}] {} PTR:{}", i, lua_typename(L, t),lua_topointer(L, i));
        }
    }
    // AXLOGD("\n");
}

int logprint(const char *__restrict __format, ...){
    va_list args;
    int result;
    char *buffer;

    // 假设最大日志长度为 1024 字节
    const int MAX_LOG_LENGTH = 1024;
    buffer = (char *)malloc(MAX_LOG_LENGTH * sizeof(char));
    if (buffer == NULL) {
        // 内存分配失败
        return -1;
    }

    // 初始化 args 以访问可变参数列表
    va_start(args, __format);
    // 使用 vsnprintf 将格式化的字符串写入 buffer
    result = vsnprintf(buffer, MAX_LOG_LENGTH, __format, args);
    // 清理 args
    va_end(args);
    std::string so=std::string(buffer);
    //  AXLOGD("---->{}",so);
    // // }
    sq.enqueue(so);
    // 释放 buffer
    free(buffer);
    return result; 

}

TOLUA_API void toluafix_stack_logdump(lua_State* L, const char* label)
{
    sq.lock();
    delete_all();       //删除所有的 内容
    luaSD_stackdump(L,logprint,label);
    std::string ssout="";
    std::string item;
    while (!sq.isEmpty()) {
        if (sq.dequeue(item)) {
           ssout=ssout+item;
        }
    }
    AXLOGD("StackDump {}{}",label,ssout);
    ssout="";
    print_rtables(logprint);
    while (!sq.isEmpty()) {
        if (sq.dequeue(item)) {
           ssout=ssout+item;
        }
    }
    AXLOGD("StackMapTable {}{}",label,ssout);
    delete_all();       //删除所有的 内容
    sq.unlock();
}



