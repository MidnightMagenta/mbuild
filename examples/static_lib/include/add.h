#ifndef EXAMPLELIB_ADD_H
#define EXAMPLELIB_ADD_H

struct add_struct {
    float addend_1;
    float addend_2;
    float output;
};

int add(struct add_struct *info);

#endif
