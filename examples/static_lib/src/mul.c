#include <mul.h>

int mul(struct mul_struct *info) {
    info->output = info->factor_1 * info->factor_2;
    return 0;
}
