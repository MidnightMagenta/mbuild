#include <context.h>
#include <lauxlib.h>
#include <lib/path.h>
#include <lua.h>
#include <lualib.h>
#include <read_script.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static int l_subdir(lua_State *L) {
    const char *path;
    char       *fullp, *absp, *realp = NULL, *pathroot;
    int         status;

    if (lua_gettop(L) != 1) { return luaL_error(L, "subdir expected 1 argument, got %d", lua_gettop(L)); }
    path = luaL_checkstring(L, 1);

    if (path[0] != '/') {
        fullp = path_join(*(char **) stack_peek(g_context->pathstack), path);
    } else {
        fullp = strdup(path);
    }

    if (!fullp) { return luaL_error(L, "failed to allocate path buffer"); }

    if (isdir(fullp)) {
        realp = path_join(fullp, "Mbuild");
        if (!realp) {
            free(fullp);
            return luaL_error(L, "failed to allocate buffer for path");
        }
        absp = abspath(realp);
        free(realp);
    } else if (isreg(fullp)) {
        absp = abspath(fullp);
    }
    free(fullp);
    if (!absp) { return luaL_error(L, "failed to resolve path '%s'", path); }

    pathroot = proot(absp);
    if (!pathroot) {
        lua_pushfstring(L, "failed to compute path root for '%s'", absp);
        free(absp);
        return lua_error(L);
    }
    if (stack_push(g_context->pathstack, &pathroot)) {
        free(pathroot);
        free(absp);
        return luaL_error(L, "failed to push path root");
    }

    status = luaL_loadfile(L, absp);
    if (status == LUA_OK) {
        lua_pushstring(L, pathroot);
        lua_pushstring(L, absp);
        status = lua_pcall(L, 2, 0, 0);
    }

    stack_pop(g_context->pathstack, NULL);
    free(pathroot);
    free(absp);

    if (status != LUA_OK) { return lua_error(L); }
    return 0;
}

int register_lua_funcs(lua_State *L) {
    static const luaL_Reg mbuildlib[] = {
            {"subdir", l_subdir},
            {NULL, NULL},
    };
    luaL_newlib(L, mbuildlib);
    lua_setglobal(L, "Mbuild");

    return 0;
}
