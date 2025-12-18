#include <build_graph.hpp>
#include <dbg_emitter.hpp>
#include <iostream>

int run(int argc, char **argv) {
    mb::BuildGraph testGraph(mb::fs::current_path());

    testGraph.artifact("foo.o");
    testGraph.artifact("foo.c");
    testGraph.artifact("bar.c");
    testGraph.artifact("baz.o");
    testGraph.artifact("baz.c");
    testGraph.artifact("someotherfile.c");
    testGraph.artifact("fun/dir/../fun.o");
    testGraph.artifact("fun.c");
    testGraph.artifact("lib.a");
    testGraph.artifact("out.elf");

    testGraph.depends("ar", "lib.a", {"fun/dir/../fun.o", "baz.o"});
    testGraph.depends("cc", "fun/dir/../fun.o", {"fun.c"});
    testGraph.depends("cc", "baz.o", {"baz.c", "someotherfile.c"});
    testGraph.depends("ld", "out.elf", {"foo.o", "lib.a"});
    testGraph.depends("cc", "foo.o", {"foo.c", "bar.c"});

    mb::DebugEmit emit;
    testGraph.emit(emit);

    return 0;
}

int main(int argc, char **argv) {
    try {
        return run(argc, argv);
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return 1;
    }
}
