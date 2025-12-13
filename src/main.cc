#include <iostream>
#include <mbuild.h>

int main(int argc, char **argv) {
    try {
        mb::MbuildApp app;
        return app.run(argc, argv);
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
