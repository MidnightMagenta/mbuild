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
