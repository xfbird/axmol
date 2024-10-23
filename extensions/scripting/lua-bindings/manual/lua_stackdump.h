/**
 * @file
 * @brief Small helper function for LUA to print stack traces from C
 * @copyright Martin Gerhardy (http://github.com/mgerhardy)
 *
 * @note
 * to create the implementation,
 *     #define LUASD_IMPLEMENTATION
 * in *one* C/CPP file that includes this file.
 *
 * @par License
 * Public domain
 *
 * @par Changelog:
 * 0.1 Initial release with some missing features and bugs, see the TODO markers in the code
 */

#ifndef LUA_STACKDUMP_H
#define LUA_STACKDUMP_H

    #ifndef luaSD_MAXDEPTH
    #define luaSD_MAXDEPTH 9
    #endif

    #ifndef luaSD_PRINT
    #include <stdio.h>
    #define luaSD_PRINT printf
    #endif

    #ifndef LUASD_API
    #define LUASD_API
    #endif

    #define __checkluastack__(s,LL)    CheckLuaStack(s,lua_gettop(LL),__FILE__, __LINE__); 

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
    #include "luaNameMap.h"                     //C Table 指针索引的名字表
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "base/Logging.h"


    extern void luaSD_stackdump (lua_State* state, luaSD_printf luasdprintf, const char * label,int showmode=0);
    extern void luaSD_stackdump_default (lua_State* state, const char * label,int showmode=0);

    #ifdef __cplusplus
    }
    #endif

    #ifndef LUASD_IMPLEMENTATION  
        #define LUASD_IMPLEMENTATION  
    #endif

    #ifdef LUASD_IMPLEMENTATION
        //----------------------------------------------------------------------------------------------
        //内部使用 字符串 子函数
            static char* safe_strdup(const char *src) {
                if (src == NULL) {
                    // fprintf(stderr, "Source string is NULL.\n");
                    return NULL;
                }

                size_t len = strlen(src) + 1; // +1 for the null terminator
                char *dest = (char *)malloc(len);

                if (dest == NULL) {
                    // fprintf(stderr, "Memory allocation failed.\n");
                    return NULL;
                }

                strncpy(dest, src, len);
                return dest;
            }

            static char* replace_crlf(const char *str) {
                if (str == NULL) {
                    return NULL;
                }

                size_t length = strlen(str);
                // 分配足够的内存，每个字符最多替换为3个字符（例如：'\n' -> '\\n'）
                char *result = (char *)malloc(3 * length + 1); // 显式类型转换
                if (result == NULL) {
                    return NULL; // 内存分配失败
                }

                char *dst = result;
                while (*str != '\0') {
                    if (*str == '\n') {
                        *dst++ = '\\';
                        *dst++ = 'n';
                    } else if (*str == '\r') {
                        *dst++ = '\\';
                        *dst++ = 'r';
                    } else {
                        *dst++ = *str;
                    }
                    str++;
                }
                *dst = '\0'; // 确保结果字符串以空字符结尾

                return result; // 返回新分配的字符串
            }

            static const char* noname = "noname";

            static int  ccount=0;
        //----------------------------------------------------------------------------------------------
        //扩展 lua API 支持 基于 class 函数 来实现
            //检查  堆栈  index 处 是否为一个Class 。标准是 有 __cname 成员 并且 __cname 成员是 字符串
            //并且 有 new 和 create  两个函数成员 才是 一个class 
            //返回 1 表示 是 Class ,栈顶 存放其所属的类 Class 表
            //返回 0 表示 不是 Class 栈顶是 错误信息
            static int sdis_class(lua_State *L, int index) {
                // AXLOGD("sdis_class top:{} in index:{}", lua_gettop(L),index);
                // [SD]  [-1] table PTR:0x2aaa7e38b90 GameStateMgr
                // [SD]  [-2] table PTR:0x2aaa5974d70 _G根
                if (lua_type(L, index) == LUA_TTABLE) {
                    // toluafix_stack_dump(L,"sdis_class in");
                    lua_getfield(L, index, ".classname");                      //index-1
                    if (lua_isstring(L, -1)){

                        lua_getfield(L, index, "__name");                      //index-1
                        if (lua_isstring(L, -1)) {
                            lua_pop(L, 1); // ——name
                            return 1;      // 存在 .classname 且 __name 是字符串确认是 Class
                        }
                        lua_pop(L, 1); // 弹出 字符串的 __name
                    }
                    lua_pop(L, 1);     // 弹出 nil 或非字符串的 .classname

                    // AXLOGD("sdis_class top:{} confirm is Table", lua_gettop(L));
                    lua_getfield(L, index, "__cname");                      //index-1
                    // get_metatable_field
                    // toluafix_stack_dump(L,"__cname");
                    // AXLOGD("sdis_class top:{} getfield cname", lua_gettop(L));
                    if (lua_isstring(L, -1)) {
                        // AXLOGD("sdis_class top:{} confirm cname is string", lua_gettop(L));
                        // 检查是否存在 create 方法
                        lua_getfield(L, index-1, "create");                  //index-2 
                        // lua_rawgetp(L, index-1,"create");
                        // toluafix_stack_dump(L,"create");
                        // AXLOGD("sdis_class top:{} getfield create", lua_gettop(L));
                        if (lua_type(L, -1) == LUA_TFUNCTION) {
                            // AXLOGD("sdis_class top:{} confirm create is Function", lua_gettop(L));
                            lua_pop(L, 1); // 弹出 create 方法
                            // 检查是否存在 new 方法
                            lua_getfield(L, index-1, "new");                //index-1
                            // lua_rawgetp(L, index-1,"new");
                            // toluafix_stack_dump(L,"new");
                            // AXLOGD("sdis_class top:{} getfield new", lua_gettop(L));
                            if (lua_type(L, -1) == LUA_TFUNCTION) {
                                // AXLOGD("sdis_class top:{}confirm new is Function", lua_gettop(L));
                                lua_pop(L, 1); // 弹出 new 方法
                                // AXLOGD("sdis_class top:{} 1 return stack str:{}", lua_gettop(L),lua_tostring(L,-1));
                                return 1; // 存在 __cname 且 create 和 new 都是函数，确认是 Class
                            }
                            // AXLOGD("sdis_class top:{} wrong new", lua_gettop(L));
                        }
                        // AXLOGD("sdis_class top:{} wrong create", lua_gettop(L));
                        lua_pop(L, 1); // 弹出 nil 或非函数的 create
                        // AXLOGD("sdis_class top:{} 1 Pop wrong create or new ", lua_gettop(L));
                    }
                    // AXLOGD("sdis_class top:{} wrong cname", lua_gettop(L));
                    lua_pop(L, 1); // 弹出 nil 或非字符串的 __cname
                    // AXLOGD("sdis_class top:{} pop wrong cname ", lua_gettop(L));
                }
                // AXLOGD("sdis_class top:{} wrong Table", lua_gettop(L));
                lua_pushnil(L); // 压入 nil 表示不是表
                // AXLOGD("sdis_class top:{} return Push nil indicate wong", lua_gettop(L));
                return 0; // 不是 Class 或者不满足条件
            }
            // 函数用于检查指定索引处的值是否是 Class
            // 主检查函数 ，用于判断指定栈位置的变量类型
            //检查  堆栈  index 处 是否为一个对象。标准是 有 class 成员 
            //返回 1 表示 是 Object ,栈顶 存放其所属的类 Class 表
            //返回 0 表示 不是
            static int sdis_object(lua_State *L, int index) {                   //[...,table]
                // AXLOGD("sdis_object top:{}  in",lua_gettop(L));                 //[...,table]
                if (lua_type(L, index) == LUA_TTABLE) {                         //[...,table]
                    // AXLOGD("sdis_object top:{}  confirm is Table",lua_gettop(L));  //[...,table]
                    lua_getfield(L, index, "class"); // 获取对象的 class 字段       //[...,table,class] //index-1
                    // lua_rawgetp(L, index,"class");
                    // AXLOGD("sdis_object top:{}  getfield Class",lua_gettop(L));//[...,table,class]
                    if (sdis_class(L, -1)) { // 使用 sdis_class 检查 class 是否是一个类 ////[...,table,class,string/nil]
                        // AXLOGD("sdis_object top:{} confirm have class",lua_gettop(L));  //[...,table,class,string]
                        // sdis_class 会将 __cname 的值留在栈顶，且会返回 1
                        lua_replace(L, -2); // 用 __cname 替换 class 表，弹出 class 表  //[...,table,string]
                        // AXLOGD("sdis_object top:{} return str:{}",lua_gettop(L),lua_tostring(L,-1));      //[...,table,string]
                        //确定 是 一个 Object 
                        //   lua_rawgetp(L, index,"class");
                        return 1; // 是 Object                                    //[...,table,string]          
                    }
                    // 如果不是类，sdis_class 会确保栈顶是 nil                      [...,table,class,nil]
                    // AXLOGD("sdis_object top:{} wrong is have class",lua_gettop(L)); //[...,table,class,nil]
                    lua_pop(L, 2); // 弹出 nil 和 class 表                         //[...,table] 
                    // AXLOGD("sdis_object top:{} Pop Wrong Class return",lua_gettop(L));        //[...,table]            
                }
                // AXLOGD("sdis_object top:{} wrong Table",lua_gettop(L));           //[...,table]
                lua_pushnil(L); // 如果失败，在栈顶压入 nil                       //[...,table,nil]
                // AXLOGD("sdis_object top:{} return Push nil indicate wong",lua_gettop(L));          //[...,table,nil]
                return 0; // 不是 Object
            }
            //alen 默认值 -1 表示 需要 计算 节点数 其他数值表示 已知节点数
            //OwnerTable 表示 已知的父节点名字
            //index 表示 需要检测的 栈的位置
            int check_table_type(lua_State *L, int index, const char* OwnerTable,const int alen=-1) {
                int type = lua_type(L, index);                      //获得类型
                const char* type_name = luaL_typename(L, index);  // 获得类型名字
                // toluafix_stack_dump(L,"check_table_type in"); 
                // AXLOGD("check_table_type top:{}  type:{}  type_name:{} alen:{} index{}", 
                                // lua_gettop(L),type,type_name,alen,index);
                int len=alen;
                if (type == LUA_TTABLE) {                           //[...,table]
                    if (len==-1){
                        len = 0;
                        lua_pushvalue(L, index);               //把 指定的数据 压入 栈顶
                        lua_pushnil(L);                        //计算有多少 子节点
                        while (lua_next(L, -2)) {
                            lua_pop(L, 1);
                            ++len;
                        }
                        lua_pop(L, 1);                         //删掉 赋值的表 栈顶  
                        // AXLOGD("check_table_type top:{}  计算出 len:{}   index:{}", lua_gettop(L),len,index);
                    } 
                    // toluafix_stack_dump(L,"check_table_type End of calc len"); //[...,table]

                    if (sdis_object(L, index)) {                        //[...,table,string/nil]
                        // 确定是一个对象
                        // AXLOGD("check_table_type top:{}  确定是 Object  Next:{}",lua_gettop(L),index-1);//[...,table,string/nil]
                        const void *tpp = lua_topointer(L, index-1);//[...,table,string/nil]
                        //显示 对象:指针(类名) 节点数
                        // lua_pushfstring(L, "%p(Object of Class:%s ) (Members:%d)", tpp, lua_tostring(L, -1), len);//[...,table,string,ostring]
                        lua_remove(L, -2); // 删除 class 字符串，栈顶是结果字符串   //[...,table,ostring]
                        // AXLOGD("check_table_type 3 return isObject top:{} OutString:{}", lua_gettop(L), lua_tostring(L, -1));
                        return 3;                                                //[...,table,ostring]   
                    }
                    // 如果不是类，sdis_class 会确保栈顶是 nil，我们再弹出 nil
                    // AXLOGD("check_table_type top:{}  not class", lua_gettop(L));//[...,table,nil]
                    lua_pop(L, 1);              // 弹出 nil 不是类              //[...,table]
                    // AXLOGD("check_table_type top:{}  not class pop nil", lua_gettop(L));
                    if (sdis_class(L, index)) {                     //[...,table,string/nil]
                        // 确定是一个类 栈顶存放类的字符串
                        // AXLOGD("check_table_type top:{}  确定是 Class  next:{}",lua_gettop(L),index-1);  //栈 增加一个
                        const void *tpp = lua_topointer(L, index-1);//位置 下探 一位
                        //显示 类 指针(类名) 节点数
                        lua_pushfstring(L, "%p(Class) (Members:%d)", tpp,len);//栈顶 增加一个 
                        lua_remove(L, -2); // 删除 __cname 字符串，栈顶是结果字符串
                        // AXLOGD("check_table_type 2 return isClass top:{} OutString:{}", lua_gettop(L), lua_tostring(L, -1));
                        return 2;
                    }
                    // 如果不是对象，sdis_object 会确保栈顶是 nil，我们再弹出 nil
                    // AXLOGD("check_table_type top:{}  not obj", lua_gettop(L));//[...,table,nil]
                    lua_pop(L, 1);              // 弹出 nil 不是 对象          //[...,table] 
                    // AXLOGD("check_table_type top:{}  确定是 Table",lua_gettop(L));
                    // AXLOGD("check_table_type top:{}  not object pop nil   next:{}", lua_gettop(L),index);
                    const void *tpp = lua_topointer(L, index);         //自身指针  //[...,table]
                    lua_pushfstring(L, "%p(Table) (Members:%d)", tpp,len);   //[...,table,ostring]
                    // lua_pushnil(L); // 压入 nil 表示是普通表
                    // lua_remove(L, -2); // 删除原来的表
                    // AXLOGD("check_table_type 1 return isTable top:{} OutString:{}", lua_gettop(L), lua_tostring(L, -1)); //[...,table,ostring]
                    return 1;                                                               //[...,table,ostring]
                }
                //
                lua_pushfstring(L, "%p({}) (Members:%d) ({})",lua_topointer(L, index),luaL_typename(L, index),len,OwnerTable);  //[...,table,ostring]
                // AXLOGD("check_table_type 0 return isOther top:{} string:{}", lua_gettop(L), lua_tostring(L, -1));//[...,table,ostring]
                return 0;                                           //[...,table,ostring]
            }  

            static void luaspIndent(luaSD_printf luasdorintf,int stackIndex,int depth,int indentLevel,int performIndent,int isGobal=0){
                if (performIndent) {
                    if (depth == 0) {
                        if (isGobal) {
                            switch (isGobal) {
                                case 1 :luasdorintf("REGISTRY| ");//全局变量输出
                                break;
                                case 2 :luasdorintf("Gobal   | ");//全局变量输出
                            }                            
                         // luasdorintf("Gobal | ");//全局变量输出
                        }else {
                            luasdorintf("%-7i | ", stackIndex);//堆栈序号 
                        }
                    } else {
                        luasdorintf("         ", stackIndex);
                    }
                    for (int i = 0; i < indentLevel - 4; ++i) {
                        luasdorintf(" ");
                    }
                    if (indentLevel >= 4) {
                        luasdorintf("\\-- ");
                    }
                }
            }

            static void CheckLuaStack(int Start,int End,const char* fname,int fline)
            {
                if (Start != End){
                    AXLOGD("出现 堆栈不平衡  开始:{}  检查点:{}  调用:{}:[{}] ",Start,End,fname,fline);
                }
             };


            // 辅助函数，用于检查给定索引处的表是否是类
            // 暂时未用 先 关闭
                // static int is_class(lua_State *L, int index) {
                //     int LC = lua_gettop(L);
                //     if (lua_type(L, index) != LUA_TTABLE) {
                //         __checkluastack__(LC,L);
                //         //CheckLuaStack(LC,lua_gettop(L));
                //         return 0; // 必须是表
                //     }

                //     // 获取元表
                //     if (!lua_getmetatable(L, index)) {
                //         //CheckLuaStack(LC,lua_gettop(L));
                //         __checkluastack__(LC,L);
                //         return 0; // 没有元表
                //     }

                //     lua_getfield(L, index, "__cname");
                //     if (!lua_isstring(L, -1))
                //         {
                //         lua_pop(L, 1); // 弹出元表和 nil 或非字符串的 __cname
                //         __checkluastack__(LC,L);
                //         // int LC1 = lua_gettop(L);
                //         // if (LC1 != LC)
                //         // {
                //         //     AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         // }
                //         return 0; // __cname 不存在或不是字符串
                //     }
                //     lua_pop(L, 1); // 弹出 __cname

                //     lua_getfield(L, index, "create");
                //     if (!lua_iscfunction(L, -1))
                //     {
                //         lua_pop(L, 1);  // 弹出元表和 nil 或非字符串的 __cname
                //         __checkluastack__(LC,L);
                //         // int LC1 = lua_gettop(L);
                //         // if (LC1 != LC)
                //         // {
                //         //     AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         // }

                //         return 0;       // __cname 不存在或不是字符串
                //     }
                //     lua_pop(L, 1);  // 弹出 __cname

                //     lua_getfield(L, index, "new");
                //     if (!lua_iscfunction(L, -1))
                //     {
                //         lua_pop(L, 1);  // 弹出元表和 nil 或非字符串的 __cname
                //         __checkluastack__(LC,L);
                //         // int LC1 = lua_gettop(L);
                //         // if (LC1 != LC)
                //         // {
                //         //     AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         // }

                //         return 0;       // __cname 不存在或不是字符串
                //     }
                //     lua_pop(L, 1); // 弹出元表
                //     __checkluastack__(LC,L);
                //     // int LC1 = lua_gettop(L);
                //     // if (LC1 != LC)
                //     // {
                //     //     AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //     // }
                //     return 1;
                // }

                // // 辅助函数，用于检查给定索引处的表是否是对象
                // static int is_object(lua_State *L, int index) {
                //     int LC = lua_gettop(L);
                //     if (lua_type(L, index) != LUA_TTABLE) {
                //         int LC1 = lua_gettop(L);
                //         if (LC1 != LC)
                //         {
                //             AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         }

                //         return 0; // 必须是表
                //     }

                //     // 获取元表
                //     if (!lua_getmetatable(L, index)) {
                //         int LC1 = lua_gettop(L);
                //         if (LC1 != LC)
                //         {
                //             AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         }

                //         return 0; // 没有元表
                //     }
                //     lua_getfield(L, index, "class");
                //     if (lua_type(L, -1) != LUA_TTABLE)
                //     {
                //         lua_pop(L, 2);  // 弹出元表和 nil 或非字符串的 __cname
                //         int LC1 = lua_gettop(L);
                //         if (LC1 != LC)
                //         {
                //             AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         }

                //         return 0;       // __cname 不存在或不是字符串
                //     }
                //     // AXLOGD("__cname :{}",lua_tostring(L, -1));
                //     lua_pop(L, 1);  // 弹出元表
                //     int LC1 = lua_gettop(L);
                //     if (LC1 != LC)
                //     {
                //         AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //     }

                //     return 1;
                // }

                // // 主函数，用于确定给定索引处的变量是普通表、类还是对象
                // static int identifyTCO(lua_State *L, int index) {
                //     int LC=lua_gettop(L);
                //     int type = lua_type(L, index);
                //     if (type != LUA_TTABLE && type != LUA_TUSERDATA) {
                //         __checkluastack__(LC,L);
                //         // return "Not a table or userdata";
                //         // int LC1 = lua_gettop(L);
                //         // if (LC1 != LC)
                //         // {
                //         //     AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         // }
                //         return 0;
                //     }

                //     if (is_class(L, index)) {
                //         // int LC1 = lua_gettop(L);
                //         // if (LC1 != LC)
                //         // {
                //         //     AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         // }
                //         __checkluastack__(LC,L);
                //         return 2;
                //         // return "Class";
                //     } else if (is_object(L, index)) {
                //         // return "Object";
                //         // int LC1 = lua_gettop(L);
                //         // if (LC1 != LC)
                //         // {
                //         //     AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         // }
                //         __checkluastack__(LC,L);                    
                //         return 3;
                //     } else {
                //         // return "Table";
                //         __checkluastack__(LC,L);
                //         // int LC1 = lua_gettop(L);
                //         // if (LC1 != LC)
                //         // {
                //         //     AXLOGD("出现 堆栈不平衡  检查{}   {} ",LC,LC1);
                //         // }
                //         return 1;
                //     }
                // }

            static int GetTableName(lua_State *L, int index) {
                // 检查指定索引处的值是否为表
                int LC=lua_gettop(L)+1;                
                if (!lua_istable(L, index)) {
                    __checkluastack__(LC,L);                    
                    return 0; // 不是表，返回 0 并把 nil 留在栈上
                }

                // 要检查的字段名称
                const char *fieldNames[] = {
                    "__name", "__cname", "_NAME", "_className", ".classname", NULL
                };

                // 首先检查表中的字段
                for (int i = 0; fieldNames[i] != NULL; i++) {
                    lua_pushstring(L, fieldNames[i]);
                    lua_rawget(L, index-1);
                    if (!lua_isnil(L, -1) && lua_isstring(L, -1)) {
                        // 如果找到字符串值，返回 1 并把字符串留在栈上
                        __checkluastack__(LC,L);
                        return 1;
                    }
                    lua_pop(L, 1); // 弹出 nil 或非字符串值
                }

                // 表中没有找到，检查元表
                if (lua_getmetatable(L, index)) {
                    for (int i = 0; fieldNames[i] != NULL; i++) {
                        lua_pushstring(L, fieldNames[i]);
                        lua_rawget(L, -2);
                        if (!lua_isnil(L, -1) && lua_isstring(L, -1)) {
                            // 如果在元表中找到字符串值，返回 1 并把字符串留在栈上
                            lua_remove(L, -2); // 移除元表
                            __checkluastack__(LC,L);                    
                            return 1;
                        }
                        lua_pop(L, 2); // 弹出 nil 和元表
                    }
                    lua_pop(L, 1); // 弹出元表
                }

                // 没有找到任何字段，栈顶返回 nil，返回 0
                lua_pushnil(L); 
                __checkluastack__(LC,L);                    
                return 0;
            } 

            //Number of members
        //----------------------------------------------------------------------------------------------
        // static 
        void luaSD_stackdumpvalue (
               // tablemap::rtablemap *rtables,
                lua_State* state, 
                luaSD_printf luasdorintf, 
                int stackIndex, 
                int depth, 
                int newline, 
                int indentLevel, 
                int performIndent,
                int isGobal,
                const char* CallTableName=NULL,
                const void* fatherfp_id=NULL)
        {
            int t;
            char* realtablename = NULL;     // 定义 realtablename为空 退出时 不为空 就要释放
            const char* tablename = noname; //内部 tablename 是不需要释放的
            if (CallTableName != NULL)      //默认值为 空指针，不为空指针 那么 就用上级传递的参数
            {
                tablename = CallTableName;
                // AXLOGD("LSD 检查当前  tablename=CallTableName={}",std::string(tablename));
            } else {
                // AXLOGD("LSD 检查当前  CallTableName 为空 tablename=默认值 :{}",std::string(tablename));
            }
           
            ccount++;
            int nstackok=lua_gettop(state);
            /* ensure that the value is on the top of the stack */
            lua_pushvalue(state, stackIndex);               //把 指定的数据 压入 栈顶
            t = lua_type(state, -1);

            char * stypename=safe_strdup(luaL_typename(state, -1));     //获得 类型
            size_t tnamlen=strlen(tablename);
            int CheckPos=lua_gettop(state);
            // AXLOGD("LSD  in stackIndex:{} depth:{} intop:{} name:{} ntype:{}  typename:{} ccount:{} curpos:{} ",
            //        stackIndex, depth,nstackok,tablename,t,std::string(stypename),ccount,CheckPos);
            // if ((stackIndex==-1)&&(depth==3)&&(CheckPos==14) && (t==6) &&(tnamlen==1)) {
            //     AXLOGW("LSD  in stackIndex:{} depth:{} top:{} name:{} t:{} typename:{}  需要中断",
            //            stackIndex, depth,CheckPos,tablename,t,std::string(stypename));
            // }
            switch (t) {
                case LUA_TNUMBER:
                    lua_pushfstring(state, "%f [%s]", lua_tonumber(state, -1), luaL_typename(state, -1));   //输出 数值以及类型
                    // AXLOGD("lSD NUMBER stackIndex:{} depth:{} top:{}  stackstr:{} ",stackIndex, depth,lua_gettop(state),lua_tostring(state,-1));
                    break;
                case LUA_TSTRING:
                    lua_pushfstring(state, "%s [%s]", lua_tostring(state, -1), luaL_typename(state, -1));   //输出 字符串
                    // AXLOGD("lSD STRING stackIndex:{} depth:{} top:{}  stackstr:{} ",stackIndex, depth,lua_gettop(state),lua_tostring(state,-1));
                    break;
                case LUA_TLIGHTUSERDATA:
                case LUA_TUSERDATA: 
                    {
                        //if (lua_getmetatable(state, -1) == 0) {             //如果没有配置 元表
                        // int ctype = identifyTCO(state, -1); 
                        // AXLOGD("LUA_TUSERDATA tablename:{}  ctype:{}",tablename,ctype);
                        lua_pushfstring(state, "[%p] [%s] (no metatable)", lua_topointer(state, -1),luaL_typename(state, -1));   //显示 指针 无元表 模式 指针显示
                        // AXLOGD("LSD UserData stackIndex:{} depth:{} top:{} name:{} return:{}", stackIndex, depth,lua_gettop(state),tablename,lua_tostring(state,-1));  
                        //} else {
                        //    /* TODO: this else branch was never reached in my tests */          
                        //    lua_getfield(state, -1, "__tostring");          //存在元表 模式。 获得 字符串名字的 函数
                        //    if (lua_isfunction(state, -1)) {                //检查是否是函数
                        //        //确定 是 函数 [ ..., function] 
                        //        if (lua_pcall(state, 0, 1, 0)) {            //那么调用它
                        //            //如果成功 [ ..., result ]  堆栈 函数的位置 换成了 结果
                        //            lua_pushfstring(state, "[%s %s] [%p]",lua_tostring(state,-1),luaL_typename(state, -1), lua_topointer(state, -1));  //输出 字符串
                        //            lua_remove(state, -2);                  //生成了 需要的 结果字符串删除 __cname 字符串，栈顶 是 结果字符串 -2 是 class 字符串 -3 是 指针
                        //        } else {
                        //            //如果失败 [ ..., function, error_object ] 
                        //            lua_pop(state, 2);                      //栈平衡 把函数 和 错误都弹出
                        //            lua_pushfstring(state, "[%s] [%p]", luaL_typename(state, -1), lua_topointer(state, -1));   //显示 指针 无元表 模式 指针显示
                        //        }
                        //    } else {
                        //        //没有得到  [ ..., nil] 
                        //        lua_pop(state, 1);                          //把 取出的函数 空值 丢弃
                        //        // AXLOGD("luaSD stack dump valueCall  :{}   top:{}",-1,lua_gettop(state));
                        //        // luaSD_stackdumpvalue(state, luasdorintf, -1, depth + 1, 1, indentLevel + 4, 1,0);  //使用 递归 去显示
                        //        // lua_pop(state, 1);                          //把数据 出栈
                        //        lua_pushfstring(state, "%s [%p]", luaL_typename(state, -1), lua_topointer(state, -1));  //显示内容入栈
                        //    }
                        //    AXLOGD("LSD UserData  stackIndex:{} depth:{} top:{} name:{} return:{}", stackIndex, depth,lua_gettop(state), std::string(tablename),lua_tostring(state,-1));  
                        //}
                        break;
                    }
                case LUA_TBOOLEAN: 
                    {
                        const int value = lua_toboolean(state, -1);
                        if (value) {
                            lua_pushliteral(state, "true");
                        } else {
                            lua_pushliteral(state, "false");
                        }
                        // AXLOGD("LSD Boolean  stackIndex:{} depth:{} top:{} name:{} return:{}", stackIndex, depth,lua_gettop(state), std::string(tablename),lua_tostring(state,-1));  
                        break;
                    }
                case LUA_TNIL:
                    lua_pushliteral(state, "nil");                
                        // AXLOGD("LSD NIL  stackIndex:{} depth:{} top:{} name:{} return:{}", stackIndex, depth,lua_gettop(state),tablename,lua_tostring(state,-1));  
                    break;
                case LUA_TFUNCTION: 
                    {
                        #if 0
                            /* TODO: somehow get function details. */
                            lua_Debug ar;
                            lua_getstack(state, -1, &ar);
                            if (lua_getinfo(state, "nSlLtuf", &ar) != 0) {
                                lua_pushfstring(state, "%s %s %d @%d %s", ar.namewhat, ar.name, ar.nups, ar.linedefined, ar.short_src);
                            } else
                        #endif
                        {
                            lua_pushfstring(state, "[%p] [%s]",lua_topointer(state, -1), luaL_typename(state, -1));
                            // AXLOGD("LSD Function  stackIndex:{} depth:{} top:{} name:{} return:{}", stackIndex, depth,lua_gettop(state),tablename,lua_tostring(state,-1));  
                        }                
                        break;
                    }
                case LUA_TTABLE: 
                    {
                        // int ctype = identifyTCO(state, -1); 
                        // AXLOGD("LUA_TTABLE tablename:{}  ctype:{}",tablename,ctype);
                        void * tablep= (void*)lua_topointer(state, -1);
                        luaspIndent(luasdorintf,stackIndex,depth,indentLevel,performIndent,isGobal);
                        int len = 0;
                        int ttn = 0;
                        lua_pushnil(state);                 //计算有多少 子节点
                        while (lua_next(state, -2)) {
                            lua_pop(state, 1);
                            ++len;
                        }
                        // AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} SubNodes:{}", 
                        //       stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),len);
                        if (!isGobal) 
                            {
                                //如果不是 Gobal 全局变量  需要检查
                                ttn = check_table_type(state,-1,tablename,len);
                            } else 
                            {
                                 const void * tpp=lua_topointer(state, -1);                      //确定性固定表 那么就一定是 Table   
                                //  lua_pushfstring(state, "%p [Table] table:(%s) (size: %d)",tpp,tablename,len);
                                 lua_pushfstring(state, "%p [Table] (member:%d)",tpp,len);
                            }
                        auto str=lua_tostring(state, -1);
                        luasdorintf("%s", str);
                        lua_pop(state, 1);                      //将 复制的     
                        // void * tablepn1= (void*)lua_topointer(state, -1);
                        // if  (tablepn1!=tablep) {
                        //      AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} SubNodes:{} 两个指针不一致 ", 
                        //            stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),len);
                        // }
                        rtablemap * tablerec=find_table(tablep);
                        // AXLOGD("LSD Table 查询 {} 结果:{}",fmt::ptr(tablep),fmt::ptr(tablerec));
                        if (tablerec)
                        {   //找到就不需要再继续处理了 
                            lua_pop(state, 1);
                            if (newline) {
                                luasdorintf("\n");                  //输出一个换行
                            }
                            //  AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} SubNodes:{} 已经处理过的表 处理完成", 
                            //        stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),len);
                            if (stypename != NULL) {
                                free(stypename); // 释放分配的内存
                            }
                            if (realtablename != NULL) {
                                free(realtablename); // 释放分配的内存
                            }
                            return;
                        }
                        if (!isGobal){                  //不是 Grobal 需要查询 __tostring 获得名字
                            lua_getfield(state, -1, "__tostring");  //存在元表 模式。 获得 字符串名字的 函数  +1
                            if (lua_isfunction(state, -1)) {        //检查是否是函数
                                //确定 是 函数 [ ..., function] 
                                if (lua_pcall(state, 0, 1, 0)==0) {  //那么调用它
                                    //如果成功 [ ..., result ]  堆栈 函数的位置 换成了 结果
                                    char coutstr[127] = "[tostr]";
                                    strncat(coutstr,lua_tostring(state,-1),120);
                                    realtablename=safe_strdup(coutstr);
                                    // AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} SubNodes:{} 改名为:{}", 
                                    //         stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),len,realtablename);
                                }  
                            }
                            //没有得到  [ ..., nil]  或者 成功 [ ..., result ] 或者 不是函数  都需要清理 一个 
                            lua_pop(state, 1);                        //把 取出的函数 空值 丢弃
                        }
                        if  (CallTableName==NULL) {
                          if (GetTableName(state, -1)==1) {
                             tablename= lua_tostring(state, -1);  
                          }
                          lua_pop(state, 1);                      //将 复制的
                        }
                        //AXLOGD("AddDict  :{}   name:{}",str,tablename);
                        add_table(tablep,tablename,fatherfp_id);
                        // AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} SubNodes:{} 记录P:{} 到map", 
                        //          stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),len,fmt::ptr(tablep));
                        const void * myfatherid=lua_topointer(state, -1);           //自己的 表id
                        //bool needlog =false;
                        //  AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} SubNodes:{} 遍历子节点", 
                        //          stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),len);
                        if ((len>0)&&(depth < luaSD_MAXDEPTH)) {
                            lua_pushnil(state);
                            int nodeindex=0;
                            int checknextpos = lua_gettop(state);
                            bool bonext = lua_next(state, -2);
                            int curpos  = lua_gettop(state);
                            char* NodeKeystr=NULL;
                            //  AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} SubNodes:{} 首轮", 
                            //      stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),len);
                            while (bonext)
                            {
                                //遍历 过程中，不可以对 Key 进行操作。
                                lua_pushvalue(state, -2);  // 复制 -2位置的 值 到 栈顶
                                if (lua_isstring(state, -1))            //如果栈顶是字符串，那么 取出字符串 
                                {
                                    NodeKeystr= safe_strdup(lua_tostring(state, -1));//Key
                                    //AXLOGD("Table :{} nodeindex:{} In stackIndex:{} depth:{} top:{} 可字符串化 TypeName:{}  String:{}",
                                    //        tablename,nodeindex, stackIndex,depth,lua_gettop(state),luaL_typename(state, -1),NodeKeystr);

                                } else
                                {
                                    lua_pushfstring(state, "[%p]", lua_topointer(state, -1));
                                    NodeKeystr= safe_strdup(lua_tostring(state, -1));//Key
                                    //AXLOGD("Table :{} nodeindex:{} In stackIndex:{} depth:{} top:{} 指针型 TypeName:{}  point:{}",
                                    //        tablename,nodeindex, stackIndex,depth,lua_gettop(state),luaL_typename(state, -2),NodeKeystr);
                                    lua_pop(state, 1);               //把 字符串取出 丢弃                                 
                                }    
                                lua_pop(state, 1);               //把 取出的函数 空值 丢弃                                 
                                luasdorintf("\n");               //输出换行。然后 输出 Key = value  直接输出 字符串
                                //  luaSD_stackdumpvalue(state, luasdorintf, -2, depth + 1, 0, indentLevel + 4, 1,0,NodeKeystr);  //是 Key 对名字赋值空 
                                //对于 表  输出  字符串
                                // lua_pushfstring(state, "%s", NodeKeystr);//lua_tostring(state, -2));   //输出 字符串
                                luaspIndent(luasdorintf,-2,depth + 1,indentLevel + 4,1,0); //做出缩进
                                luasdorintf("%s =",NodeKeystr);                             //输出换行。然后输出Key=value
                                //luasdorintf(" = ");
                                luaSD_stackdumpvalue(state, luasdorintf, -1, depth + 1, 0, indentLevel + 4, 0,0,NodeKeystr,myfatherid);
                                lua_pop(state, 1);          //把 值 删除 然后 继续遍历
                                bonext = lua_next(state, -2);
                                curpos = lua_gettop(state);
                                nodeindex++;
                                //  AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} CurNodes:{}  Nodes:{} 继续:{} NodeKeystr:{}", 
                                //      stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),nodeindex,len,bonext,NodeKeystr);
                            }
                            
                            int LC = lua_gettop(state);
                            // 尝试获取该表的元表
                            if (lua_getmetatable(state, -1)) {
                                // 检查是否成功获取到元表
                                // AXLOGD("lua_getmetatable 内 堆栈 {} ", lua_gettop(state)); 
                                if (!lua_isnil(state, -1)) {
                                    // 元表存在，使用 lua_pushfstring 将元表指针转换为字符串
                                    // AXLOGD("isnil 堆栈 {} ", lua_gettop(state)); 
                                    luasdorintf("\n");                              //输出换行。然后 输出 Key = value  直接输出 字符串
                                    luaspIndent(luasdorintf,-2,depth + 1,indentLevel + 4,1,0); //做出缩进
                                    // lua_pushfstring(state, "metas->%p",lua_topointer(state, -1));                                    
                                    // luasdorintf("%s", lua_tostring(state, -1));
                                    luasdorintf("MetaTable->");
                                    luaSD_stackdumpvalue(state, luasdorintf, -1, depth + 1, 0, indentLevel + 4, 0,0,NULL,myfatherid);
                                    // AXLOGD("isnil 堆栈 pop 前 {} ", lua_gettop(state)); 
                                    lua_pop(state, 1); // 弹出 字符串
                                    // AXLOGD("isnil 堆栈 pop 后 {} ", lua_gettop(state)); 
                                } 
                                // 元表不存在，什么都不做
                                // AXLOGD("isnil 堆栈 判断出 后 {} ", lua_gettop(state)); 
                                // // lua_pop(state, 1); // 弹出 元表
                                // AXLOGD("isnil 堆栈 判断出  pop后 {} ", lua_gettop(state)); 
                            }
                            // lua_pop(state, 1); // 弹出 元表
                            __checkluastack__(LC,state);

                            // int ctype = identifyTCO(state, -1); 
                            // AXLOGD("LUA_TUSERDATA tablename:{}  ctype:{}",tablename,ctype);
                            if (NodeKeystr!=NULL){
                                free(NodeKeystr);
                            };
                        }
                        //  AXLOGD("LSD Table stackIndex:{} depth:{} top:{} TableName:{} TypeName:{} SubNodes:{} 遍历完成 ", 
                        //         stackIndex, depth,lua_gettop(state),tablename,std::string(stypename),len);
                        lua_pop(state, 1);
                        if (newline) {
                            luasdorintf("\n");
                        }
                        if (nstackok != lua_gettop(state)) { 
                            // AXLOGW("luaSD stack dump value 退出  栈不平衡 进入 stackIndex :{} depth:{} 进入栈:{}  出栈:{} type:{} ",
                            //        stackIndex,depth,nstackok,lua_gettop(state),std::string(stypename));
                            if (stypename != NULL) {
                                free(stypename); // 释放分配的内存
                            }
                            if (realtablename != NULL) {
                                free(realtablename); // 释放分配的内存
                            }
                            return;
                        }
                        if (stypename != NULL) {
                                free(stypename); // 释放分配的内存
                        }
                        if (realtablename != NULL) {
                            free(realtablename); // 释放分配的内存
                        }
                        return;
                    }
                default:
                    lua_pushfstring(state, "[%p] (%s)", lua_topointer(state, -1),luaL_typename(state, -1));
                    //  AXLOGD("LSD stackIndex:{} depth:{} top:{} TypeName:{}  输出:{}", 
                    //             stackIndex, depth,lua_gettop(state),std::string(stypename),lua_tostring(state, -1));
                    break;
            }
            {
                int width = 20;                                //默认宽度 20 
                if (performIndent) {                           //如果需要缩进显示         
                    if (depth == 0) {
                        luasdorintf("%-5i | ", stackIndex);    //深度 0 显示 最外层的 变量序号
                    } else {
                        luasdorintf("       ", stackIndex);    //缩进  第一位值
                    }
                    for (int i = 0; i < indentLevel - 4; ++i) {
                        luasdorintf(" ");                      //按照缩进等级 输出 空格位
                    }
                    if (indentLevel >= 4) {                    //缩进层 大于 4 的，显示 \\--
                        luasdorintf("\\-- ");                  //缩进 大于4 层 
                    }
                    width -= indentLevel;                      //宽度 减去  缩进等级 
                }

                char *escaped_str = replace_crlf(lua_tostring(state, -1));  //将特殊字符进行转换 
                if (escaped_str != NULL) {
                    luasdorintf("%-*s", width, escaped_str);
                    // AXLOGD("LSD stackIndex:{} depth:{} top:{} tablename:{} stype:{} 需要输出:{}", 
                    //         stackIndex, depth, lua_gettop(state),std::string(tablename),std::string(stypename),std::string(escaped_str));
                    free(escaped_str); // 释放分配的内存
                }
            }
            if (newline) {
                luasdorintf("\n");                  //输出一个换行
            }
            /* pop the string and the reference copy from the stack to restore the original state */
            lua_pop(state, 2);                      //两个 量出栈，一个是 自身的 字符串表示，一个是 自己。
            if (nstackok != lua_gettop(state)) { 
                AXLOGW("luaSD stack dump value 退出  栈不平衡 进入 stackIndex :{} depth:{} 进入栈:{}  出栈:{}",stackIndex,depth,nstackok,lua_gettop(state));
            } else {
            }
            if (stypename != NULL) {
                    free(stypename); // 释放分配的内存
            }
            if (realtablename != NULL) {
                    free(realtablename); // 释放分配的内存
            }
        }

        LUASD_API void luaSD_stackdump (lua_State* state, luaSD_printf luasdprintf, const char * label,int showmode) {
            const int top = lua_gettop(state);
            luasdprintf("\n--------------------start-of-stacktrace----------------\n");
            luasdprintf("Check in %s index | details (%i entries)\n",label, top);
            // lua_pushglobaltable(state); //输出 全局变量。    1
            // luaSD_stackdumpvalue(state, luasdprintf, -1, 0, 1, 1, 1,1,"_G_Grobal");
            //对于 根部的 注册表 进行输出
            if (showmode==1) {
                lua_pushvalue(state, LUA_REGISTRYINDEX);
                luaSD_stackdumpvalue(state, luasdprintf, -1, 0, 1, 1, 1,1,"_REGISTRYINDEX");
                lua_pop(state, 1);             //删除 全局变量
            }
            //下面输出当前堆栈    
            int i;
            char namebuffer[50];
            for (i = -1; i >= -top; i--) {
                try {
                    snprintf(namebuffer,sizeof(namebuffer), "stack_%d", i);
                    luaSD_stackdumpvalue(state, luasdprintf, i, 0, 1, 1, 1,0,namebuffer);
                } catch (...) {
                    AXLOGD("luaSD stack dump value catch:{}",i);
                    break;    
                }
            }
            
            luasdprintf("----------------------end-of-stacktrace----------------\n\n");
        }
        LUASD_API void luaSD_stackdump_default (lua_State* state, const  char * label,int showmode) {
            luaSD_stackdump(state, luaSD_PRINT,label,showmode);
        }
    #endif

#endif

