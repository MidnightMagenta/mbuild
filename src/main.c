#include <context.h>

int main(int argc, char **argv) {
    int     res = 0;
    context ctx;

    res = ctx_init(&ctx, (struct opts){argc, argv});
    if (res) { goto fail0; }

fail0:
    ctx_destroy(&ctx);
    return res;
}
