#include <api.hpp>
#include <build_graph.hpp>
#include <dbg_emitter.hpp>
#include <iostream>
#include <terminal.hpp>

int run(int argc, char **argv) {
    mb::api::g_buildGraph.build("cc").input("foo.c").output("foo.o", true);
    mb::api::g_buildGraph.build("cc").input("bar.c").output("bar.o", true);
    mb::api::g_buildGraph.build("cc").input("baz.c").output("baz.o", true);
    mb::api::g_buildGraph.build("ld").inputs({"foo.o", "bar.o", "baz.o"}).output("foo.elf", true);

    mb::DebugEmit emit;
    emit.begin();
    mb::api::g_buildGraph.emit(emit);
    emit.end();

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
