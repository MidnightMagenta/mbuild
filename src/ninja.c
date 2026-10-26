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

void write_rule(FILE *outf, const struct rule *r) {
    assert(r);
    assert(outf);

    fprintf(outf, "rule %s\n", r->name);
    for (size_t i = 0; i < R_COUNT; i++) {
        if (r->attr[i]) { fprintf(outf, "  %s = %s\n", rule_keys[i], r->attr[i]); }
    }
}

int write_ninja(context *ctx) {
    char *outpath = path_join(ctx->build_dir, "build.ninja");
    FILE *outf    = fopen(outpath, "w+");
    int   res;

    if (!outf) {
        res = errno;
        pr_err(&ctx->log, "Failed to open output file %s with: %s", outpath, strerror(res));
        free(outpath);
        return EC_RUNTIMEERR;
    }
    free(outpath);

    for (size_t i = 0; i < ml_dynarr_size(ctx->rules); i++) {
        //
        write_rule(outf, *((struct rule **) ml_dynarr_get(ctx->rules, i)));
    }

    fclose(outf);
    return EC_NORMAL;
}
