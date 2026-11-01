#ifndef ARGS_H
#define ARGS_H

#include <getopt.h>
#include <stddef.h>

enum {
    OPT_CDIR      = 'C',
    OPT_VERBOSITY = 'V',
    OPT_VERSION   = 'v',
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

#define SHORTOPT_STRING ":C:V:vh"

#endif
