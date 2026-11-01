#include <add.h>

int add(struct add_struct *info) {
    info->output = info->addend_1 + info->addend_2;
    return 0;
}
