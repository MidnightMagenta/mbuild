#ifndef ARGS_H
#define ARGS_H

#include <getopt.h>
#include <stddef.h>

enum {
    OPT_CDIR      = 'C',
    OPT_VERBOSITY = 'v',
    OPT_VERSION   = 'V',
    OPT_HELP      = 'h',
    OPT_LONGONLY  = 256,
};

static struct option longopts[] = {
        {"cdir", required_argument, NULL, OPT_CDIR},
        {"verbosity", required_argument, NULL, OPT_VERBOSITY},
        {"version", no_argument, NULL, OPT_VERSION},
        {"help", no_argument, NULL, OPT_HELP},
        {0, 0, 0, 0},
};

#define SHORTOPT_STRING ":C:v:Vh"

#endif
