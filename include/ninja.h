#ifndef NINJA_H
#define NINJA_H

#include <context.h>
#include <ninja_types.h>
#include <stdio.h>


struct emit_ctx {
    char *root;
    char *build;
    int   failed;
};

char *path_to_build_rel(const char *root, const char *build, const char *p);

void write_var(FILE *outf, const struct kv *v);
void write_pool(FILE *outf, const struct kv *v);
void write_default(FILE *outf, const char *d);
void write_rule(FILE *outf, const struct rule *r);
void write_build(FILE *outf, struct emit_ctx *e, const struct build *b);

int write_ninja(context *ctx);

#endif
