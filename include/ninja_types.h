#ifndef NINJA_TYPES_H
#define NINJA_TYPES_H

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

struct rule *rule_new(const char *name);
void         rule_dtor(void *r);
int          get_rule_idx(const char *k);

#endif
