#ifndef ARGS_H
#define ARGS_H

#include <getopt.h>
#include <stddef.h>

enum {
    OPT_CDIR      = 'C',
    OPT_VERBOSITY = 'v',
    OPT_LONGONLY  = 256,
};

static struct option longopts[] = {
        {"cdir", required_argument, NULL, OPT_CDIR},
        {"verbosity", required_argument, NULL, OPT_VERBOSITY},
};

#define SHORTOPT_STRING ":C:v:"

#endif
