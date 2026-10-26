#include <context.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <ninja_types.h>
#include <path.h>
#include <read_script.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static int l_subdir(lua_State *L) {
    const char *path;
    char       *fullp = NULL, *absp = NULL, *realp = NULL, *pathroot = NULL;
    int         status;

    if (lua_gettop(L) != 1) { return luaL_error(L, "'subdir' expected 1 argument, got %d", lua_gettop(L)); }
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

int l_file(lua_State *L) {
    const char *p;
    char       *relp;

    if (lua_gettop(L) != 1) { return luaL_error(L, "'file' expected 1 argument, got %d", lua_gettop(L)); }
    p = luaL_checkstring(L, 1);

    if (p[0] != '/') {
        char *fullp = path_join(*(char **) stack_peek(g_context->pathstack), p);
        relp        = relpath(fullp, g_context->root_dir);
        free(fullp);
    } else {
        relp = relpath(p, g_context->root_dir);
    }


    lua_pushstring(L, relp);
    free(relp);

    return 1;
}

static int in_list(const char *k, const char **keys, size_t key_cnt) {
    for (size_t i = 0; i < key_cnt; i++) {
        if (!strcmp(k, keys[i])) { return 1; }
    }
    return 0;
}

int l_rule(lua_State *L) {
    const char  *name;
    struct rule *r;

    if (lua_gettop(L) != 2) { return luaL_error(L, "rule: expected 2 arguments, got %d", lua_gettop(L)); }
    name = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);

    for (lua_pushnil(L); lua_next(L, 2); lua_pop(L, 1)) {
        const char *k = luaL_checkstring(L, -2);
        if (!in_list(k, rule_keys, R_COUNT)) { return luaL_error(L, "rule '%s': unknown attribute '%s'", name, k); }
    }

    lua_getfield(L, 2, "command");
    if (lua_isnil(L, -1)) {
        free(r);
        return luaL_error(L, "rule '%s': missing 'command'", name);
    }
    lua_pop(L, 1);

    r = rule_new(name);
    if (!r) { return luaL_error(L, "rule '%s: failed to allocate rule"); }

    for (size_t i = 0; i < R_COUNT; i++) {
        lua_getfield(L, 2, rule_keys[i]);
        if (!lua_isnil(L, -1)) {
            if (!lua_isstring(L, -1)) {
                rule_dtor(r);
                return luaL_error(L, "rule '%s': value for %s must be a string", name, rule_keys[i]);
            }
            r->attr[get_rule_idx(rule_keys[i])] = strdup(lua_tostring(L, -1));
        }
        lua_pop(L, 1);
    }

    ml_dynarr_push_back(g_context->rules, &r);

    lua_pushstring(L, name);
    return 1;
}

int register_lua_funcs(lua_State *L) {
    static const luaL_Reg mbuildlib[] = {
            {"subdir", l_subdir},
            {"file", l_file},
            {"rule", l_rule},
            {NULL, NULL},
    };
    luaL_newlib(L, mbuildlib);
    lua_setglobal(L, "mb");

    return 0;
}
