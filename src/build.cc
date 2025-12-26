#include <getopt.h>
#include <global_config.hpp>
#include <iostream>
#include <terminal.hpp>

void build_usage() {
    std::cout << "Invoke the build backend with [args]\n";
    // general usage
    std::cout << mb::term::bold << "Usage:\n" << mb::term::reset;
    std::cout << "\t" << mb::g_toolName << " build [flags] -- [args]\n";

    // flags
    std::cout << mb::term::bold << "Flags:\n" << mb::term::reset;
    std::cout << "\t-h, --help\n" << "\t\tPrints this help message\n";
    std::cout << "\t-b, --backend [arg]\n"
              << "\t\tSets the backend to be used. Valid options are: ninja.\n\t\tDefault = ninja\n";
}

int build_exec(const char* cmd, int argc, char **argv){

}

int do_build(int argc, char **argv) {
    optind                      = 1;
    static struct option opts[] = {
            {"help", no_argument, nullptr, 'h'},
            {"backend", required_argument, nullptr, 'b'},
            {0, 0, 0, 0},
    };

    std::string backend = "ninja";

    int opt;
    while ((opt = getopt_long(argc, argv, "hC:b:", opts, nullptr)) != -1) {
        switch (opt) {
            case 'h':
                build_usage();
                return 0;
            case 'b':
                backend = optarg;
                break;
            case '?':
                [[fallthrough]];
            default:
                build_usage();
                return 1;
        }
    }

    return build_exec(backend.c_str(), argc - optind, argv + optind);
}
