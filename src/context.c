#include <args.h>
#include <context.h>
#include <ecode.h>
#include <errno.h>
#include <lib/stack.h>
#include <logging.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

context *g_context;

static void rdenv(struct config *cfg) {
    char *no_color;
    int   tty;

    cfg->color = true;
    no_color   = getenv("NO_COLOR");
    if (no_color != NULL && no_color[0] != '\0') { cfg->color = false; }
    if (!isatty(fileno(stderr))) { cfg->color = false; }
}

static int readargs(struct config *cfg, const struct opts *opts) {
    int opt, idx, npos;

    while ((opt = getopt_long(opts->argc, opts->argv, SHORTOPT_STRING, longopts, &idx)) != -1) {
        switch (opt) {
            case 0:
                break;
            case 'v': {
                char *end;
                errno  = 0;
                long n = strtol(optarg, &end, 10);
                if (errno != 0 || end == optarg || *end != '\0') {
                    fprintf(stderr, "invalid argument for %s\n", opts->argv[optind - 1]);
                    return EC_VALUERRR;
                }
                if (n < 0 || n > LOG_MAX) {
                    fprintf(stderr, "verbose must be between 0 and %d\n", LOG_MAX);
                    return EC_VALUERRR;
                }
                cfg->v = n;
            } break;
            case 'C':
                if (chdir(optarg)) {
                    fprintf(stderr, "Could not change directory to %s\n", optarg);
                    return EC_VALUERRR;
                }
                break;
            case ':':
                fprintf(stderr, "missing argument for options %s\n", opts->argv[optind - 1]);
                return EC_VALUERRR;
            case '?':
                fprintf(stderr, "unknown option %s\n", opts->argv[optind - 1]);
                return EC_VALUERRR;
        }
    }

    npos = opts->argc - optind;
    if (npos != 1) {
        // TODO:  usage
        return EC_VALUERRR;
    }

    cfg->rfile = opts->argv[optind];

    return EC_NORMAL;
}

static void free_str(void *s) {
    free(*(char **) s);
}

int ctx_init(context *ctx, const struct opts opts) {
    int res;

    // startup init
    cfg_default(&ctx->cfg);
    rdenv(&ctx->cfg);
    if ((res = readargs(&ctx->cfg, &opts))) { return res; }

    log_init(&ctx->log, ctx->cfg.v, stderr, ctx->cfg.color);

    // build config parser state init
    ctx->pathstack = stack_create(sizeof(char *), free_str, 0);

    return 0;
}

void ctx_destroy(context *ctx) {
    if (ctx->pathstack) { stack_free(ctx->pathstack); }
}
