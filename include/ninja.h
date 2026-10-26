#ifndef NINJA_H
#define NINJA_H

#include <context.h>
#include <ninja_types.h>
#include <stdio.h>

void write_rule(FILE *outf, const struct rule *r);

int write_ninja(context *ctx);

#endif
