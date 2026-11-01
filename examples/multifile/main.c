#include "shared_header.h"
#include <stdio.h>

extern void mf_fun_0();
extern void mf_fun_1();
extern void mf_fun_2();
extern void mf_fun_3();
extern void mf_fun_4();

long sh_uvs[SH_UV_CNT];

void gen_uvs() {
    for (int i = 0; i < SH_UV_CNT; i++) { sh_uvs[i] = UV_VALUE_START + (i * UV_VALUE_OFF); }
}

int main() {
    gen_uvs();

    mf_fun_0();
    mf_fun_1();
    mf_fun_2();
    mf_fun_3();
    mf_fun_4();

    printf("Main's unique value was: %ld\n", sh_uvs[SH_UV_MAIN]);

    return 0;
}
