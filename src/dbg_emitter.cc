#include <dbg_emitter.hpp>
#include <iostream>

namespace term {
constexpr const char *reset = "\033[0m";
constexpr const char *red   = "\033[31m";
constexpr const char *green = "\033[32m";

constexpr const char *bold = "\033[1m";
}// namespace term
//
void mb::DebugEmit::end() {
    size_t max_edge_width = 0;
    for (const auto &e : m_edges) {
        size_t w = 0;
        for (const auto &o : e.out) { w += o.size() + 1; }
        max_edge_width = std::max(max_edge_width, w);
    }

    for (const auto &e : m_edges) {
        size_t w = 0;
        for (const auto &o : e.out) {
            std::cout << o << " ";
            w += o.size() + 1;
        }

        std::cout << std::string(max_edge_width - w, ' ');
        std::cout << term::red << term::bold << "-[" << e.rule << "]-> " << term::reset;

        for (const auto &i : e.in) { std::cout << i << " "; }
        std::cout << "\n";
    }
}
