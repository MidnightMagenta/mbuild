#ifndef EXAMPLES_MULTIFILE_HEADER_H
#define EXAMPLES_MULTIFILE_HEADER_H

#include <stdio.h>

#define UV_VALUE_START 0
#define UV_VALUE_OFF   5

enum {
    SH_UV_MAIN,
    SH_UV_MF0,
    SH_UV_MF1,
    SH_UV_MF2,
    SH_UV_MF3,
    SH_UV_MF4,
    SH_UV_CNT,
};

extern long sh_uvs[SH_UV_CNT];

#define GEN_MF_FUNC(idx, uv_idx)                                                                                       \
    void mf_fun_##idx() {                                                                                              \
        printf("Multifile function %d's unique value was %ld\n", idx, sh_uvs[uv_idx]);                                 \
    }

#endif
