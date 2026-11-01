#include <add.h>
#include <stdio.h>

int main() {
    struct add_struct addinfo = {1.0f, 5.0f, 0.0f};
    add(&addinfo);

    printf("add %f\n", addinfo.output);
    return 0;
}
