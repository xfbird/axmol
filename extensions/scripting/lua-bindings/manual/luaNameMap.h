#ifndef LUA_NameMap_H
#define LUA_NameMap_H

    #include "uthash.h"
    // #include <stdio.h>
    // #include <stdlib.h>
    #include <string.h>
    #include "base/Logging.h"

    typedef int (*luaSD_printf) (const char *__restrict __format, ...);

    //内部使用的 map 函数
        struct rtablemap {
            const void *p;                    /* key */
            char name[40];
            const void *fp;                   /* key */
            UT_hash_handle hh;          /* makes this structure hashable */
        };

        struct rtablemap *rtables = NULL;

        static void add_table(const void *funcpoint_id, const char *name,const void *fatherfp_id=NULL)
        {
            struct rtablemap *s;

            HASH_FIND_PTR(rtables, &funcpoint_id, s);  /* id already in the hash? */
            if (s == NULL) {
                s = (struct rtablemap*)malloc(sizeof *s);
                if (s == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    exit(EXIT_FAILURE);
                }
                s->p = funcpoint_id;
                s->fp= fatherfp_id;
                HASH_ADD_PTR(rtables, p, s);  /* id is the key field */
            }
            if (name != NULL) {
                strncpy(s->name, name, sizeof(s->name) - 1);
                s->name[sizeof(s->name) - 1] = '\0'; // 确保字符串以空字符终止
            }
        }

        static struct rtablemap *find_table(const void *funcpoint_id)
        {
            struct rtablemap *s;

            HASH_FIND_PTR(rtables, &funcpoint_id, s);  /* s: output pointer */
            return s;
        }

        static const char * getnamebyfpid (const void *funcpoint_id)
        {
            struct rtablemap *s;

            HASH_FIND_PTR(rtables, &funcpoint_id, s);  /* id already in the hash? */
            if (s == NULL) {
                return "Root";
            }
            return s->name;
        }


        static void delete_table(struct rtablemap *table)
        {
            HASH_DEL(rtables, table);  /* user: pointer to deletee */
            free(table);
        }

        void delete_all()
        {
            struct rtablemap *current_table, *tmp;

            HASH_ITER(hh, rtables, current_table, tmp) {
                HASH_DEL(rtables, current_table);  /* delete it (rtables advances to next) */
                free(current_table);             /* free it */
            }
        }

        void print_rtables(luaSD_printf luasdprintf)
        {
            struct rtablemap *s;
            luasdprintf("\nTable Map Print \n");
            for (s = rtables; s != NULL; s = (struct rtablemap*)(s->hh.next)) 
            {
                // AXLOGD("table funcpoint [{}]--->{}", (void*)s->p, s->name);
                const void * vpp=s->fp;
                luasdprintf("   [%p]:%s Owner is[%p(%s)] \n",(void*)s->p, s->name,(void*)vpp,getnamebyfpid(vpp));
            }
            luasdprintf("Table Map Print End \n");
        }


#endif

