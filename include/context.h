#ifndef CONTEXT_H
#define CONTEXT_H

#include <config.h>
#include <lib/stack.h>
#include <logging.h>

struct opts {
    int    argc;
    char **argv;
};

typedef struct {
    // startup configuration
    struct config cfg;
    struct log    log;

    // build config parser state
    stack_t pathstack;
} context;

extern context *g_context;

int  ctx_init(context *ctx, const struct opts opts);
void ctx_destroy(context *ctx);

#endif
