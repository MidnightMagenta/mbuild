#include <build_context.hpp>
#include <cassert>
#include <dbg_emitter.hpp>
#include <filesystem>
#include <getopt.h>
#include <global_config.hpp>
#include <iostream>
#include <terminal.hpp>
#include <utils.hpp>

extern void evaluate(const char *);

int usage() {
    std::cout << mb::term::bold << "Usage:\n" << mb::term::reset;
    std::cout << "\t" << mb::g_toolName << " <subcommand> [flags] [args]\n";
    return 0;
}

int run(int argc, char **argv) {
    int                   opt;
    std::filesystem::path rootDir = std::filesystem::current_path();

    optind = 1;

    static struct option opts[] = {
            {"help", no_argument, nullptr, 'h'},
            {"change-dir", required_argument, nullptr, 'C'},
            {0, 0, 0, 0},
    };

    while ((opt = getopt_long(argc, argv, "hC:", opts, nullptr)) != -1) {
        switch (opt) {
            case 'h':
                usage();
                break;
            case 'C': {
                rootDir = std::filesystem::weakly_canonical(optarg);
                if (!std::filesystem::exists(rootDir)) {
                    throw std::runtime_error("Cannot change path to a non existant directory " + rootDir.string());
                }
                break;
            }
            case '?':
                [[fallthrough]];
            default:
                usage();
                return 1;
        }
    }

    evaluate(rootDir.string().c_str());
    mb::DebugEmit e;
    mb::g_buildContext.emit(e);

    return 0;
}

int main(int argc, char **argv) {
    try {
        mb::g_toolName = argv[0];
        return run(argc, argv);
    } catch (const std::runtime_error &e) {
        std::cerr << mb::term::red << "Error: " << mb::term::reset << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << mb::term::red << "Exception: " << mb::term::reset << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << mb::term::red << "Unknown error" << mb::term::reset << std::endl;
        return 1;
    }
}
