#include <api.hpp>
#include <build_graph.hpp>
#include <dbg_emitter.hpp>
#include <iostream>
#include <terminal.hpp>

int run(int argc, char **argv) {

    mb::api::g_buildGraph.set_rule_association("cc", {".c"});
    mb::api::g_buildGraph.set_rule_association("cxx", {".cc", ".cpp", ".cxx"});

    mb::api::source("foo.c");
    mb::api::source("bar.c");
    mb::api::source("baz.cpp");

    mb::api::g_buildGraph.file("foo.elf");
    mb::api::g_buildGraph.build("foo.elf", "ld", {"foo.o", "bar.o", "baz.o"});

    mb::DebugEmit emit;
    mb::api::g_buildGraph.emit(emit);

    return 0;
}

int main(int argc, char **argv) {
    try {
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
