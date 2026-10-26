#ifndef CONTEXT_H
#define CONTEXT_H

#include <config.h>
#include <dynarr.h>
#include <logging.h>
#include <stack.h>

struct opts {
    int    argc;
    char **argv;
};

typedef struct {
    // startup configuration
    struct config cfg;
    struct log    log;

    // build config parser state
    const char *root_dir;
    const char *build_dir;
    stack_t     pathstack;
    ml_dynarr   rules;
} context;

extern context *g_context;

int  ctx_init(context *ctx, const struct opts opts);
void ctx_destroy(context *ctx);

#endif
