#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

struct config {
    const char *rfile;
    bool        color;
    int         v;
};

static inline void cfg_default(struct config *cfg) {
    cfg->rfile = "Mbuild";
    cfg->color = false;
    cfg->v     = 1;
}

#endif
