#ifndef MBUILD_CONTEXT_H
#define MBUILD_CONTEXT_H

#include <build_graph.hpp>
#include <string>
#include <vector>

namespace mb {
class BuildContext {
public:
    BuildContext() {}
    ~BuildContext() {}

    void build(const std::string &rule, const std::vector<std::string> &ins, const std::vector<std::string> outs) {
        build(rule, to_paths(ins), to_paths(outs));
    }

    void build(const std::string &rule, const std::vector<fs::path> &ins, const std::vector<fs::path> outs) {
        m_graph.build(rule).inputs(ins).outputs(outs, true);
    }

    void emit(Emitter &e) {
        m_graph.emit(e);
    }

private:
    std::vector<fs::path> to_paths(const std::vector<std::string> &p);

private:
    BuildGraph m_graph;
};

inline BuildContext g_buildContext;
}// namespace mb

#endif
