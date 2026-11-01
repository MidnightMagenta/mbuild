#include <assert.h>
#include <dynarr.h>
#include <ecode.h>
#include <errno.h>
#include <logging.h>
#include <ninja.h>
#include <path.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_var(FILE *outf, const struct kv *v) {
    fprintf(outf, "%s = %s\n", v->k, v->v);
}

void write_pool(FILE *outf, const struct kv *v) {
    fprintf(outf, "pool %s\n  depth = %s\n", v->k, v->v);
}

void write_rule(FILE *outf, const struct rule *r) {
    fprintf(outf, "rule %s\n", r->name);
    for (size_t i = 0; i < R_COUNT; i++) {
        if (r->attr[i]) { fprintf(outf, "  %s = %s\n", rule_keys[i], r->attr[i]); }
    }
}

static char *to_build_rel(const struct emit_ctx *e, const char *p) {
    char *joined = path_join(e->root, p);
    if (!joined) { return NULL; }

    char *abs = abspath(joined);
    free(joined);
    if (!abs) { return NULL; }

    char *rel = relpath(abs, e->build);
    free(abs);
    return rel;
}

static void put_path(FILE *f, struct emit_ctx *e, const char *p) {
    char *conv = NULL;

    if (p[0] != '/') {
        conv = to_build_rel(e, p);
        if (!conv) {
            e->failed = 1;
            return;
        }
        p = conv;
    }

    for (; *p; p++) {
        if ((*p == ' ') || (*p == ':') || (*p == '$')) { fputc('$', f); }
        fputc(*p, f);
    }
    free(conv);
}

static void put_paths(FILE *f, struct emit_ctx *e, const char *prefix, ml_dynarr s) {
    size_t n = ml_dynarr_size(s);
    if (!n) { return; }
    if (prefix) { fputs(prefix, f); }
    for (size_t i = 0; i < n; i++) {
        fputc(' ', f);
        put_path(f, e, *(char **) ml_dynarr_get(s, i));
    }
}

static int kv_cmp(const void *a, const void *b) {
    return strcmp(((const struct kv *) a)->k, ((const struct kv *) b)->k);
}

void write_build(FILE *outf, struct emit_ctx *e, const struct build *b) {
    assert(b);
    assert(outf);

    fputs("build", outf);
    put_paths(outf, e, NULL, b->out);
    put_paths(outf, e, " |", b->implicit_out);
    fprintf(outf, ": %s", b->rule);
    put_paths(outf, e, NULL, b->inputs);
    put_paths(outf, e, " |", b->implicit);
    put_paths(outf, e, " ||", b->order_only);
    fputc('\n', outf);

    if (b->pool) { fprintf(outf, "  pool = %s\n", b->pool); }

    size_t     nv   = ml_dynarr_size(b->vars);
    struct kv *vars = ml_dynarr_data(b->vars);
    if (nv) { qsort(vars, nv, sizeof(*vars), kv_cmp); }
    for (size_t j = 0; j < nv; j++) { fprintf(outf, "  %s = %s\n", vars[j].k, vars[j].v); }
}

static void write_regen(FILE *outf, struct emit_ctx *e, context *ctx) {
    char *exe     = realpath("/proc/self/exe", NULL);
    char *relroot = relpath(ctx->root_script, e->build);
    if (!exe || !relroot) {
        e->failed = 1;
        free(exe);
        free(relroot);
        return;
    }

    fputs("rule regen\n", outf);
    fprintf(outf, "  command = %s -C . %s\n", exe, relroot);
    fputs("  description = REGEN $out\n", outf);
    fputs("  generator = 1\n\n", outf);

    fputs("build build.ninja:", outf);
    fputs(" regen", outf);
    for (size_t i = 0; i < ml_dynarr_size(ctx->scripts); i++) {
        fputc(' ', outf);
        put_path(outf, e, *(char **) ml_dynarr_get(ctx->scripts, i));
    }
    fputs(" | ", outf);
    put_path(outf, e, exe);
    fputs("\n  pool = console\n\n", outf);

    free(exe);
    free(relroot);
}

int write_ninja(context *ctx) {
    struct emit_ctx e = {0};

    char *outpath = path_join(ctx->build_dir, "build.ninja");
    FILE *outf    = outpath ? fopen(outpath, "w") : NULL;

    if (!outf) {
        int res = outpath ? errno : ENOMEM;
        pr_err(&ctx->log, "Failed to open output file %s with: %s", outpath ? outpath : "build.ninja", strerror(res));
        free(outpath);
        return EC_RUNTIMEERR;
    }

    e.root  = abspath(ctx->root_dir);
    e.build = abspath(ctx->build_dir);
    if (!e.root || !e.build) {
        pr_err(&ctx->log, "Out of memory while preparing %s", outpath);
        free(e.root);
        free(e.build);
        fclose(outf);
        free(outpath);
        return EC_RUNTIMEERR;
    }

    fputs("# regen\n", outf);
    write_regen(outf, &e, ctx);

    fputs("# global variables\n", outf);
    for (size_t i = 0; i < ml_dynarr_size(ctx->vars); i++) { write_var(outf, ml_dynarr_get(ctx->vars, i)); }

    fputs("\n# pools\n", outf);
    for (size_t i = 0; i < ml_dynarr_size(ctx->pools); i++) { write_pool(outf, ml_dynarr_get(ctx->pools, i)); }

    fputs("\n# rules\n", outf);
    for (size_t i = 0; i < ml_dynarr_size(ctx->rules); i++) {
        write_rule(outf, *((struct rule **) ml_dynarr_get(ctx->rules, i)));
        fputc('\n', outf);
    }

    fputs("# edges\n", outf);
    for (size_t i = 0; i < ml_dynarr_size(ctx->build); i++) { write_build(outf, &e, ml_dynarr_get(ctx->build, i)); }

    fputs("\n# defaults\n", outf);
    if (ml_dynarr_size(ctx->defaults)) {
        put_paths(outf, &e, "default", ctx->defaults);
        fputc('\n', outf);
    }

    int failed = ferror(outf) || e.failed;
    if (fclose(outf) != 0) { failed = 1; }

    if (failed) { pr_err(&ctx->log, "Failed to write %s", outpath); }

    free(e.root);
    free(e.build);
    free(outpath);
    return failed ? EC_RUNTIMEERR : EC_NORMAL;
}
