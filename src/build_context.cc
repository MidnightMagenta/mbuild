#include <build_context.hpp>

std::vector<mb::fs::path> mb::BuildContext::to_paths(const std::vector<std::string> &p) {
    std::vector<fs::path> paths;
    paths.reserve(p.size());

    for (const auto &s : p) { paths.emplace_back(s); }

    return paths;
}
