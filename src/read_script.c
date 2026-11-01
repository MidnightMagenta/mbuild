#include <context.h>
#include <ecode.h>
#include <errno.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <ninja.h>
#include <ninja_types.h>
#include <path.h>
#include <read_script.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static struct project *cur_project(void) {
    return *(struct project **) stack_peek(g_context->projects);
}

static char *resolve_abs(const char *root, const char *p) {
    char *joined = (p[0] == '/') ? strdup(p) : path_join(root, p);
    if (!joined) { return NULL; }

    char *abs = abspath(joined);
    free(joined);
    return abs;
}

static int mkdir_p(const char *path) {
    char *tmp = strdup(path);
    if (!tmp) { return -1; }

    for (char *p = tmp + 1; *p; p++) {
        if (*p != '/') { continue; }
        *p = '\0';
        if ((mkdir(tmp, 0777) != 0) && (errno != EEXIST)) {
            free(tmp);
            return -1;
        }
        *p = '/';
    }

    int res = ((mkdir(tmp, 0777) != 0) && (errno != EEXIST)) ? -1 : 0;
    free(tmp);
    return res;
}

static int l_subdir(lua_State *L) {
    const char *path;
    char       *fullp = NULL, *absp = NULL, *realp = NULL, *pathroot = NULL, *script = NULL;
    int         base, nret = 0, status;

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

    script = strdup(absp);
    if (!script) {
        stack_pop(g_context->pathstack, NULL);
        free(pathroot);
        free(absp);
        return luaL_error(L, "subdir: oom");
    }

    if (ml_dynarr_push_back(g_context->scripts, &script)) {
        free(script);
        stack_pop(g_context->pathstack, NULL);
        free(pathroot);
        free(absp);
        return luaL_error(L, "failed to record script path");
    }

    base = lua_gettop(L);
    nret = 0;

    status = luaL_loadfile(L, absp);
    if (status == LUA_OK) {
        lua_pushstring(L, pathroot);
        lua_pushstring(L, absp);
        status = lua_pcall(L, 2, LUA_MULTRET, 0);
        if (status == LUA_OK) { nret = lua_gettop(L) - base; }
    }

    stack_pop(g_context->pathstack, NULL);
    free(pathroot);
    free(absp);

    if (status != LUA_OK) { return lua_error(L); }
    return nret;
}

static int l_subproject(lua_State *L) {
    if ((lua_gettop(L) < 1) || (lua_gettop(L) > 2)) {
        return luaL_error(L, "subproject: expected 1 or 2 arguments, got %d", lua_gettop(L));
    }
    const char *path = luaL_checkstring(L, 1);
    const char *bdir = luaL_optstring(L, 2, "build");

    char *target = resolve_abs(cur_project()->root, path);
    if (!target) { return luaL_error(L, "subproject: oom"); }
    char *script = isdir(target) ? path_join(target, "Mbuild") : strdup(target);
    free(target);
    if (!script || !isreg(script)) {
        free(script);
        return luaL_error(L, "subproject: no script found at '%s'", path);
    }

    for (size_t i = 0; i < ml_dynarr_size(g_context->subprojects); i++) {
        struct project *known = *(struct project **) ml_dynarr_get(g_context->subprojects, i);
        if (strcmp(known->script, script) != 0) { continue; }
        free(script);
        if (known->ref == LUA_NOREF) { return luaL_error(L, "subproject: circular reference to '%s'", path); }
        if (known->ref == LUA_REFNIL) {
            lua_pushnil(L);
        } else {
            lua_rawgeti(L, LUA_REGISTRYINDEX, known->ref);
        }
        return 1;
    }

    char *root     = proot(script);
    char *build    = root ? resolve_abs(root, bdir) : NULL;
    char *ninja    = build ? path_join(build, "mbuild.sub.ninja") : NULL;
    char *cwd      = getcwd(NULL, 0);
    char *pathroot = root ? strdup(root) : NULL;

    struct project *sub = (ninja && cwd) ? project_new(root, build, ninja, script) : NULL;
    free(root);
    free(build);
    free(ninja);
    if (!sub || !pathroot || (mkdir_p(sub->build) != 0)) {
        project_free(sub);
        free(pathroot);
        free(cwd);
        free(script);
        return luaL_error(L, "subproject: cannot set up '%s'", path);
    }

    sub->ref = LUA_NOREF;
    if (ml_dynarr_push_back(g_context->subprojects, &sub)) {
        project_free(sub);
        free(pathroot);
        free(cwd);
        free(script);
        return luaL_error(L, "subproject: oom");
    }

    if (stack_push(g_context->projects, &sub)) {
        free(pathroot);
        free(cwd);
        free(script);
        return luaL_error(L, "subproject: oom");
    }
    if (stack_push(g_context->pathstack, &pathroot)) {
        stack_pop(g_context->projects, NULL);
        free(pathroot);
        free(cwd);
        free(script);
        return luaL_error(L, "subproject: oom");
    }
    if (ml_dynarr_push_back(g_context->scripts, &script)) {
        stack_pop(g_context->pathstack, NULL);
        stack_pop(g_context->projects, NULL);
        free(pathroot);
        free(cwd);
        free(script);
        return luaL_error(L, "subproject: oom");
    }

    int status;
    if (chdir(sub->root) != 0) {
        lua_pushfstring(L, "subproject: cannot enter '%s': %s", sub->root, strerror(errno));
        status = LUA_ERRRUN;
    } else {
        status = luaL_loadfile(L, script);
        if (status == LUA_OK) {
            lua_pushstring(L, sub->root);
            lua_pushstring(L, script);
            status = lua_pcall(L, 2, 1, 0);
        }
        if ((chdir(cwd) != 0) && (status == LUA_OK)) {
            lua_pop(L, 1);
            lua_pushstring(L, "subproject: cannot restore the working directory");
            status = LUA_ERRRUN;
        }
    }

    stack_pop(g_context->pathstack, NULL);
    stack_pop(g_context->projects, NULL);
    free(pathroot);
    free(cwd);

    if (status != LUA_OK) { return lua_error(L); }

    lua_pushvalue(L, -1);
    sub->ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 1;
}

static int l_file(lua_State *L) {
    const char *p;
    char       *relp;

    if (lua_gettop(L) != 1) { return luaL_error(L, "'file' expected 1 argument, got %d", lua_gettop(L)); }
    p = luaL_checkstring(L, 1);

    if (p[0] != '/') {
        char *fullp = path_join(*(char **) stack_peek(g_context->pathstack), p);
        relp        = relpath(fullp, cur_project()->root);
        free(fullp);
    } else {
        relp = relpath(p, cur_project()->root);
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

static int l_rule(lua_State *L) {
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
    if (lua_isnil(L, -1)) { return luaL_error(L, "rule '%s': missing 'command'", name); }
    lua_pop(L, 1);

    r = rule_new(name);
    if (!r) { return luaL_error(L, "rule '%s': failed to allocate rule", name); }

    for (size_t i = 0; i < R_COUNT; i++) {
        lua_getfield(L, 2, rule_keys[i]);
        if (!lua_isnil(L, -1)) {
            if (!lua_isstring(L, -1)) {
                rule_dtor(&r);
                return luaL_error(L, "rule '%s': value for %s must be a string", name, rule_keys[i]);
            }
            r->attr[get_rule_idx(rule_keys[i])] = strdup(lua_tostring(L, -1));
        }
        lua_pop(L, 1);
    }

    ml_dynarr_push_back(cur_project()->rules, &r);

    lua_pushstring(L, name);
    return 1;
}

#define FLATTEN_MAX_DEPTH 200

static lua_Integer flatten_into(lua_State *L, int src, int dst, lua_Integer n, int depth) {
    lua_Integer len;
    src = lua_absindex(L, src);
    dst = lua_absindex(L, dst);

    if (depth > FLATTEN_MAX_DEPTH) luaL_error(L, "flatten: nesting too deep (cyclic table?)");
    luaL_checkstack(L, 3, "flatten: stack overflow");

    len = (lua_Integer) lua_rawlen(L, src);
    for (lua_Integer i = 1; i <= len; i++) {
        lua_rawgeti(L, src, i);
        if (lua_type(L, -1) == LUA_TTABLE) {
            n = flatten_into(L, -1, dst, n, depth + 1);
            lua_pop(L, 1);
        } else {
            lua_rawseti(L, dst, ++n);
        }
    }
    return n;
}

static lua_Integer push_flattened(lua_State *L, int idx) {
    idx = lua_absindex(L, idx);
    lua_newtable(L);
    return flatten_into(L, idx, lua_gettop(L), 0, 0);
}

static int l_flatten(lua_State *L) {
    if (lua_gettop(L) != 1) { return luaL_error(L, "flatten: expected 1 argument, got %d", lua_gettop(L)); }
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_newtable(L);
    flatten_into(L, 1, 2, 0, 0);
    return 1;
}

static void oom(lua_State *L) {
    luaL_error(L, "build: oom");
}

static void sv_push(lua_State *L, ml_dynarr a, const char *s) {
    if (s[0] == '\0') { luaL_error(L, "empty path is not allowed"); }

    char *copy = resolve_abs(cur_project()->root, s);
    if (!copy) { oom(L); }
    if (ml_dynarr_push_back(a, &copy)) {
        free(copy);
        oom(L);
    }
}
static void kv_push(lua_State *L, ml_dynarr a, const char *k, const char *v) {
    struct kv e = {strdup(k), strdup(v)};
    if (!e.k || !e.v || ml_dynarr_push_back(a, &e)) {
        kv_dtor(&e);
        oom(L);
    }
}

static struct build *ctx_add(lua_State *L, ml_dynarr ctx) {
    struct build b = {0};
    b.out          = ml_dynarr_new(0, sizeof(char *), str_dtor);
    b.implicit_out = ml_dynarr_new(0, sizeof(char *), str_dtor);
    b.inputs       = ml_dynarr_new(0, sizeof(char *), str_dtor);
    b.implicit     = ml_dynarr_new(0, sizeof(char *), str_dtor);
    b.order_only   = ml_dynarr_new(0, sizeof(char *), str_dtor);
    b.vars         = ml_dynarr_new(0, sizeof(struct kv), kv_dtor);

    if (ml_dynarr_push_back(ctx, &b)) {
        build_dtor(&b);
        oom(L);
    }
    return ml_dynarr_get(ctx, ml_dynarr_size(ctx) - 1);
}

static void read_str(lua_State *L, const char *key, void *dst) {
    if (lua_type(L, -1) != LUA_TSTRING) { luaL_error(L, "build: '%s' must be a string", key); }
    *(char **) dst = strdup(lua_tostring(L, -1));
}

static void read_list(lua_State *L, const char *key, void *dst) {
    ml_dynarr a = *(ml_dynarr *) dst;
    int       t = lua_type(L, -1);
    if (t == LUA_TSTRING) {
        sv_push(L, a, lua_tostring(L, -1));
        return;
    }
    if (t != LUA_TTABLE) { luaL_error(L, "build: '%s' must be a string or a list of strings", key); }

    lua_Integer n = push_flattened(L, -1);
    for (lua_Integer i = 1; i <= n; i++) {
        lua_rawgeti(L, -1, i);
        if (lua_type(L, -1) != LUA_TSTRING) { luaL_error(L, "build: '%s' must contain only strings", key); }
        sv_push(L, a, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

static void read_vars(lua_State *L, const char *key, void *dst) {
    ml_dynarr a = *(ml_dynarr *) dst;
    if (lua_type(L, -1) != LUA_TTABLE) { luaL_error(L, "build: '%s' must be a table", key); }
    int t = lua_absindex(L, -1);
    for (lua_pushnil(L); lua_next(L, t); lua_pop(L, 1)) {
        if (lua_type(L, -2) != LUA_TSTRING || lua_type(L, -1) != LUA_TSTRING) {
            luaL_error(L, "build: '%s' must map strings to strings", key);
        }
        kv_push(L, a, lua_tostring(L, -2), lua_tostring(L, -1));
    }
}

typedef void (*reader_fn)(lua_State *L, const char *key, void *dst);
struct field {
    const char *name;
    reader_fn   read;
    size_t      off;
    int         required;
};

#define FIELD(name, reader, member, req) {name, reader, offsetof(struct build, member), req}
static const struct field fields[] = {
        FIELD("rule", read_str, rule, 1),
        FIELD("out", read_list, out, 1),
        FIELD("implicit_out", read_list, implicit_out, 0),
        FIELD("inputs", read_list, inputs, 0),
        FIELD("implicit", read_list, implicit, 0),
        FIELD("order_only", read_list, order_only, 0),
        FIELD("vars", read_vars, vars, 0),
        FIELD("pool", read_str, pool, 0),
};
#define NFIELDS (sizeof fields / sizeof *fields)

static size_t find_field(const char *k) {
    for (size_t i = 0; i < NFIELDS; i++) {
        if (!strcmp(k, fields[i].name)) { return i; }
    }
    return NFIELDS;
}

static void push_strlist(lua_State *L, ml_dynarr a) {
    size_t n = ml_dynarr_size(a);
    lua_createtable(L, (int) n, 0);
    for (size_t i = 0; i < n; i++) {
        lua_pushstring(L, *(char **) ml_dynarr_get(a, i));
        lua_rawseti(L, -2, (lua_Integer) i + 1);
    }
}

static int l_build(lua_State *L) {
    ml_dynarr ctx = cur_project()->edges;
    luaL_checktype(L, 1, LUA_TTABLE);

    struct build *b    = ctx_add(L, ctx);
    unsigned      seen = 0;

    for (lua_pushnil(L); lua_next(L, 1); lua_pop(L, 1)) {
        if (lua_type(L, -2) != LUA_TSTRING) { return luaL_error(L, "build: attribute names must be strings"); }

        const char *k = lua_tostring(L, -2);
        size_t      i = find_field(k);
        if (i == NFIELDS) { return luaL_error(L, "build: unknown attribute '%s'", k); }

        fields[i].read(L, k, (char *) b + fields[i].off);
        seen |= 1u << i;
    }

    for (size_t i = 0; i < NFIELDS; i++) {
        if (fields[i].required && !(seen & (1u << i))) {
            return luaL_error(L, "build: missing required attribute '%s'", fields[i].name);
        }
    }

    push_strlist(L, b->out);
    return 1;
}

static int l_var(lua_State *L) {
    if (lua_gettop(L) != 2) { return luaL_error(L, "var: expected 2 arguments, got %d", lua_gettop(L)); }
    const char *k = luaL_checkstring(L, 1);
    const char *v = luaL_checkstring(L, 2);

    kv_push(L, cur_project()->vars, k, v);

    return 0;
}

static int l_pool(lua_State *L) {
    if (lua_gettop(L) != 2) return luaL_error(L, "pool: expected 2 arguments, got %d", lua_gettop(L));

    const char *name = luaL_checkstring(L, 1);
    char        buf[32];
    const char *depth;

    switch (lua_type(L, 2)) {
        case LUA_TSTRING:
            depth = lua_tostring(L, 2);
            break;

        case LUA_TNUMBER: {
            int         isint;
            lua_Integer d = lua_tointegerx(L, 2, &isint);
            if (!isint) return luaL_argerror(L, 2, "pool depth must be an integer");
            if (d < 0) return luaL_argerror(L, 2, "pool depth must not be negative");
            snprintf(buf, sizeof buf, "%lld", (long long) d);
            depth = buf;
            break;
        }

        default:
            return luaL_argerror(L, 2, "pool depth must be a string or integer");
    }

    kv_push(L, cur_project()->pools, name, depth);
    return 0;
}

static int l_default(lua_State *L) {
    if (lua_gettop(L) != 1) return luaL_error(L, "default: expected 1 argument, got %d", lua_gettop(L));

    ml_dynarr defaults = cur_project()->defaults;

    if (lua_type(L, 1) == LUA_TSTRING) {
        sv_push(L, defaults, lua_tostring(L, 1));
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) { return luaL_argerror(L, 1, "expected a string or list of strings"); }

    lua_Integer n = push_flattened(L, 1);
    for (lua_Integer i = 1; i <= n; i++) {
        lua_rawgeti(L, 2, i);
        if (lua_type(L, -1) != LUA_TSTRING) { return luaL_error(L, "default: element %d is not a string", (int) i); }
        sv_push(L, defaults, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    return 0;
}

static int l_builddir(lua_State *L) {
    if (lua_gettop(L) != 0) { return luaL_error(L, "get_builddir takes no arguments. Got %d", lua_gettop(L)); }

    struct project *cur = cur_project();
    char           *rel = relpath(cur->build, cur->root);
    if (!rel) { return luaL_error(L, "get_builddir: oom"); }

    lua_pushstring(L, rel);
    free(rel);
    return 1;
}

static int l_rel(lua_State *L) {
    if (lua_gettop(L) != 1) { return luaL_error(L, "rel: expected 1 argument, got %d", lua_gettop(L)); }
    const char *p = luaL_checkstring(L, 1);

    if (p[0] == '/') {
        lua_pushvalue(L, 1);
        return 1;
    }

    char *abs = resolve_abs(cur_project()->root, p);
    char *rel = abs ? relpath(abs, g_context->top->build) : NULL;
    free(abs);
    if (!rel) { return luaL_error(L, "rel: out of memory"); }

    lua_pushstring(L, rel);
    free(rel);
    return 1;
}

static char *exe_dir() {
    char    buf[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n <= 0) { return NULL; }
    buf[n] = '\0';

    char *slash = strrchr(buf, '/');
    if (!slash) { return NULL; }
    *slash = '\0';
    return strdup(buf);
}

static int setup_lua_path(lua_State *L) {
    char *dir = exe_dir();
    if (!dir) { return -1; }

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    lua_pushfstring(L, "%s/llib/?.lua;%s/llib/?/init.lua;%s", dir, dir, lua_tostring(L, -1));
    lua_setfield(L, -3, "path");
    lua_pop(L, 2);

    free(dir);
    return 0;
}

int register_lua_funcs(lua_State *L) {
    static const luaL_Reg mbuildlib[] = {
            {"subdir", l_subdir},
            {"subproject", l_subproject},
            {"file", l_file},
            {"rule", l_rule},
            {"flatten", l_flatten},
            {"build", l_build},
            {"var", l_var},
            {"pool", l_pool},
            {"default", l_default},
            {"builddir", l_builddir},
            {"rel", l_rel},
            {NULL, NULL},
    };
    luaL_newlib(L, mbuildlib);
    lua_setglobal(L, "mb");

    if (setup_lua_path(L)) { return EC_RUNTIMEERR; }

    return 0;
}
