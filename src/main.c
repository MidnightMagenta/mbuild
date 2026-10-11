#include <context.h>
#include <ecode.h>
#include <lauxlib.h>
#include <lib/path.h>
#include <lib/stack.h>
#include <lua.h>
#include <lualib.h>
#include <read_script.h>
#include <stdlib.h>
#include <unistd.h>

static int read_script(context *ctx) {
    lua_State *L;
    char      *absp, *rootdir;
    int        res = EC_NORMAL, status;

    L = luaL_newstate();
    if (!L) { return EC_ALLOCERR; }
    luaL_openlibs(L);
    register_lua_funcs(L);

    absp = abspath(ctx->cfg.rfile);
    if (!absp) {
        res = EC_ALLOCERR;
        goto fail0;
    }
    rootdir = proot(absp);
    if (!rootdir) {
        res = EC_ALLOCERR;
        goto fail1;
    }
    if ((res = stack_push(ctx->pathstack, &rootdir))) { goto fail2; }

    status = luaL_loadfile(L, absp);
    if (status == LUA_OK) {
        lua_pushstring(L, rootdir);
        lua_pushstring(L, absp);
        status = lua_pcall(L, 2, 0, 0);
    }

    if (status != LUA_OK) {
        pr_err(&ctx->log, "%s\n", lua_tolstring(L, -1, NULL));
        res = EC_RUNTIMEERR;
    }

    stack_pop(ctx->pathstack, NULL);
fail2:
    free(rootdir);
fail1:
    free(absp);
fail0:
    lua_close(L);
    return res;
}

int main(int argc, char **argv) {
    int     res = 0;
    context ctx;

    res = ctx_init(&ctx, (struct opts){argc, argv});
    if (res) { goto fail0; }
    g_context = &ctx;

    res = read_script(&ctx);
    if (res) { goto fail0; }

fail0:
    ctx_destroy(&ctx);
    return res;
}
