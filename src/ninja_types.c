#include <ninja_types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void build_dtor(void *p) {
    struct build *b = p;
    free(b->rule);
    free(b->pool);
    ml_dynarr_free(b->out);
    ml_dynarr_free(b->implicit_out);
    ml_dynarr_free(b->inputs);
    ml_dynarr_free(b->implicit);
    ml_dynarr_free(b->order_only);
    ml_dynarr_free(b->vars);
}

struct rule *rule_new(const char *name) {
    struct rule *r = malloc(sizeof(struct rule));
    if (!r) { return NULL; }
    memset(r, 0, sizeof(struct rule));
    r->name = strdup(name);
    if (!r->name) {
        free(r);
        return NULL;
    }

    return r;
}

void rule_dtor(void *r) {
    struct rule *rule = *((struct rule **) r);
    free(rule->name);
    for (size_t i = 0; i < R_COUNT; i++) {
        if (rule->attr[i]) { free(rule->attr[i]); }
    }
    free(rule);
}

int get_rule_idx(const char *k) {
    for (size_t i = 0; i < R_COUNT; i++) {
        if (!strcmp(k, rule_keys[i])) { return i; }
    }
    return R_COUNT;
}

int get_build_idx(const char *k) {
    for (size_t i = 0; i < B_COUNT; i++) {
        if (!strcmp(k, build_keys[i])) { return i; }
    }
    return B_COUNT;
}

struct project *project_new(const char *root, const char *build, const char *ninja, const char *script) {
    struct project *p = calloc(1, sizeof(*p));
    if (!p) { return NULL; }

    p->root     = strdup(root);
    p->build    = strdup(build);
    p->ninja    = strdup(ninja);
    p->script   = script ? strdup(script) : NULL;
    p->rules    = ml_dynarr_new(0, sizeof(struct rule *), rule_dtor);
    p->edges    = bctx_new();
    p->vars     = vars_new();
    p->pools    = pools_new();
    p->defaults = defaults_new();

    if (!p->root || !p->build || !p->ninja || (script && !p->script) || !p->rules || !p->edges || !p->vars ||
        !p->pools || !p->defaults) {
        project_free(p);
        return NULL;
    }
    return p;
}

void project_free(struct project *p) {
    if (!p) { return; }
    free(p->root);
    free(p->build);
    free(p->ninja);
    free(p->script);
    if (p->rules) { ml_dynarr_free(p->rules); }
    if (p->edges) { ml_dynarr_free(p->edges); }
    if (p->vars) { ml_dynarr_free(p->vars); }
    if (p->pools) { ml_dynarr_free(p->pools); }
    if (p->defaults) { ml_dynarr_free(p->defaults); }
    free(p);
}

void project_dtor(void *p) {
    project_free(*(struct project **) p);
}
