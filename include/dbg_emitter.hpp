#ifndef MBUILD_DEBUG_EMIT_H
#define MBUILD_DEBUG_EMIT_H

#include <emit.hpp>

namespace mb {
class DebugEmit : public Emitter {
public:
    DebugEmit() {}
    ~DebugEmit() {}

    virtual void begin() {}

    virtual void edge(const std::string              &rule,
                      const std::vector<std::string> &out,
                      const std::vector<std::string> &in) {
        m_edges.emplace_back(Edge{.rule = rule, .out = out, .in = in});
    }

    virtual void end();

private:
    struct Edge {
        std::string              rule;
        std::vector<std::string> out;
        std::vector<std::string> in;
    };

private:
    std::vector<Edge> m_edges;
};
}// namespace mb

#endif
