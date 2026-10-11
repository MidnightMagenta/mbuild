#ifndef CONTEXT_H
#define CONTEXT_H

#include <config.h>
#include <logging.h>

struct opts {
    int    argc;
    char **argv;
};

typedef struct {
    struct config cfg;
    struct log    log;
} context;

int  ctx_init(context *ctx, const struct opts opts);
void ctx_destroy(context *ctx);

#endif
