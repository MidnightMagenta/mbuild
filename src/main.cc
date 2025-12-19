#include <build_graph.hpp>
#include <dbg_emitter.hpp>
#include <iostream>
#include <terminal.hpp>

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

    testGraph.depends_a("ar", "lib.a", {"fun/dir/../fun.o", "baz.o"});
    testGraph.depends_a("cc", "fun/dir/../fun.o", {"fun.c"});
    testGraph.depends_a("cc", "baz.o", {"baz.c", "someotherfile.c"});
    testGraph.depends_a("ld", "out.elf", {"foo.o", "lib.a"});
    testGraph.depends_a("cc", "foo.o", {"foo.c", "bar.c"});

    testGraph.set_var("test", "some set of values");
    testGraph.set_var("secondTest", "--two --flags");
    testGraph.append_var("secondTest", "--three");
    testGraph.append_var("thirdTest", "--value");

    testGraph.set_default_rules();

    mb::DebugEmit emit;
    testGraph.emit(emit);

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
