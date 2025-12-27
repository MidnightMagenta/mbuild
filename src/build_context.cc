#include <build_context.hpp>

void mb::BuildContext::emit(mb::Emitter &e) {
    e.begin();
    for (const auto &[k, v] : m_vars) { e.global_var(k, v); }
    m_rules.emit(e);
    m_graph.emit(e);
    e.end();
}

std::vector<mb::fs::path> mb::BuildContext::to_paths(const std::vector<std::string> &p) {
    std::vector<fs::path> paths;
    paths.reserve(p.size());

    for (const auto &s : p) { paths.emplace_back(s); }

    return paths;
}
