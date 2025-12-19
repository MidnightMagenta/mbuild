#ifndef MBUILD_DEBUG_EMIT_H
#define MBUILD_DEBUG_EMIT_H

#include <emit.hpp>
#include <iomanip>
#include <iostream>

namespace mb {
class DebugEmit : public Emitter {
public:
    DebugEmit() {}
    ~DebugEmit() {}

    void begin() override {}

    void global_var(const std::string &name, const std::string &value) override {
        std::cout << std::left << std::setw(10) << name << " = " << value << "\n";
    }

    void rule(const Rule &rule) override;

    void edge(const std::string              &rule,
              const std::vector<std::string> &out,
              const std::vector<std::string> &in) override {
        m_edges.emplace_back(Edge{.rule = rule, .out = out, .in = in});
    }

    void end() override;

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
