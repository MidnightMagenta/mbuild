#ifndef NINJA_TYPES_H
#define NINJA_TYPES_H

#include <dynarr.h>
#include <stdlib.h>

enum {
    R_COMMAND,
    R_DESCRIPTION,
    R_DEPFILE,
    R_DEPS,
    R_MSVC_DEPS_PREFIX,
    R_GENERATOR,
    R_RESTAT,
    R_RSPFILE,
    R_RSPFILE_CONTENT,
    R_POOL,
    R_COUNT,
};

static const char *rule_keys[R_COUNT] = {
        "command",
        "description",
        "depfile",
        "deps",
        "msvc_deps_prefix",
        "generator",
        "restat",
        "rspfile",
        "rspfile_content",
        "pool",
};

struct rule {
    char *name;
    char *attr[R_COUNT];
};

enum {
    B_RULE,
    B_OUT,
    B_IMPLICIT_OUT,
    B_IN,
    B_IMPLICIT_IN,
    B_ORDER_ONLY,
    B_VAR,
    B_POOL,
    B_COUNT,
};

static const char *build_keys[B_COUNT] = {
        "rule",
        "out",
        "implicit_out",
        "inputs",
        "implicit",
        "order_only",
        "vars",
        "pool",
};

struct kv {
    char *k, *v;
};

struct build {
    char     *rule;
    ml_dynarr out, implicit_out, inputs, implicit, order_only; /* of char* */
    ml_dynarr vars;                                            /* of struct kv */
    char     *pool;
};

static void str_dtor(void *p) {
    free(*(char **) p);
}

static void kv_dtor(void *p) {
    struct kv *e = p;
    free(e->k);
    free(e->v);
}

void build_dtor(void *p);

static inline ml_dynarr bctx_new() {
    return ml_dynarr_new(0, sizeof(struct build), build_dtor);
}

static inline ml_dynarr vars_new() {
    return ml_dynarr_new(0, sizeof(struct kv), kv_dtor);
}

static inline ml_dynarr pools_new() {
    return ml_dynarr_new(0, sizeof(struct kv), kv_dtor);
}

static inline ml_dynarr defaults_new() {
    return ml_dynarr_new(0, sizeof(char *), str_dtor);
}

struct rule *rule_new(const char *name);
void         rule_dtor(void *r);
int          get_rule_idx(const char *k);

int get_build_idx(const char *k);

struct project {
    char     *root;
    char     *build;
    char     *ninja;
    char     *script;
    int       ref;
    ml_dynarr vars, pools, rules, edges, defaults;
};

struct project *project_new(const char *root, const char *build, const char *ninja, const char *script);
void            project_free(struct project *p);
void            project_dtor(void *p);

#endif
