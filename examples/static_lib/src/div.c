#include <div.h>

int div(struct div_struct *info) {
    info->output = info->divident / info->divisor;
    return 0;
}
