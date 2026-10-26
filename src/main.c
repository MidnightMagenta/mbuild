#include <context.h>
#include <ecode.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <ninja.h>
#include <path.h>
#include <read_script.h>
#include <stack.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int read_script(context *ctx) {
    lua_State *L;
    char      *absp = NULL, *rootdir = NULL;
    int        res = EC_NORMAL, status;

    L = luaL_newstate();
    if (!L) { return EC_ALLOCERR; }
    luaL_openlibs(L);
    register_lua_funcs(L);

    if (isdir(ctx->cfg.rfile)) {
        char *realp = path_join(ctx->cfg.rfile, "Mbuild");
        if (!realp) {
            pr_err(&ctx->log, "could not resolve path '%s'", ctx->cfg.rfile);
            goto fail0;
        }
        absp = abspath(realp);
        free(realp);
    } else if (isreg(ctx->cfg.rfile)) {
        absp = abspath(ctx->cfg.rfile);
    }

    if (!absp) {
        pr_err(&ctx->log, "failed to resolve path '%s'", ctx->cfg.rfile);
        res = EC_ALLOCERR;
        goto fail0;
    }
    rootdir = proot(absp);
    if (!rootdir) {
        pr_err(&ctx->log, "failed to resolve path root for '%s'", absp);
        res = EC_ALLOCERR;
        goto fail1;
    }
    if ((res = stack_push(ctx->pathstack, &rootdir))) { goto fail2; }

    ctx->root_dir = strdup(rootdir);

    status = luaL_loadfile(L, absp);
    if (status == LUA_OK) {
        lua_pushstring(L, rootdir);
        lua_pushstring(L, absp);
        status = lua_pcall(L, 2, 0, 0);
    }

    if (status != LUA_OK) {
        pr_err(&ctx->log, "%s", lua_tolstring(L, -1, NULL));
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

    res = write_ninja(&ctx);

fail0:
    ctx_destroy(&ctx);
    return res;
}
