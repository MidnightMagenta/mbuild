#include <cassert>
#include <cstring>
#include <global_config.hpp>
#include <iostream>
#include <terminal.hpp>
#include <utils.hpp>

int do_help(int argc, char **argv) {
    std::cout << mb::term::bold << "Usage:\n" << mb::term::reset;
    std::cout << "\t" << mb::g_toolName << " <subcommand> [flags] [args]\n";
    return 0;
}

extern int do_configure(int argc, char **argv);
extern int do_build(int argc, char **argv);

#define DECL_CMD(n) {#n, static_cast<int (*)(int, char **)>(do_##n)}

struct {
    const char *name;
    int (*run)(int, char **);
} constexpr g_commands[]{
        DECL_CMD(help),
        DECL_CMD(build),
        DECL_CMD(configure),
};

int run(int argc, char **argv) {
    const int    sub_argc    = argc - 1;
    char **const sub_argv    = argv + 1;
    const char  *help_argv[] = {"help", nullptr};

    if (argc < 2) {
        std::cerr << "Invalid arguments\nUsage:\n";
        do_help(1, const_cast<char **>(help_argv));
        return 1;
    }

    for (const auto &cmd : g_commands) {
        assert(cmd.name != nullptr);
        assert(cmd.run != nullptr);

        if (strcmp(cmd.name, sub_argv[0]) == 0) { return cmd.run(sub_argc, sub_argv); }
    }

    std::cerr << mb::term::red << "Error: " << mb::term::reset << "Invalid subcommand: " << argv[1] << "\nUsage:\n";
    do_help(1, const_cast<char **>(help_argv));

    return 1;
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
