#include <sub.h>

int sub(struct sub_struct *info) {
    info->output = info->minuend - info->subtrahend;
    return 0;
}
