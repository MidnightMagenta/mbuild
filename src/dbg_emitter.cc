#include <dbg_emitter.hpp>
#include <terminal.hpp>

void mb::DebugEmit::rule(const mb::BuildRules::Rule &rule) {
    std::cout << "Rule: " << rule.m_name << "\n";
    std::cout << "\tVars: {\n";
    for (const auto &[k, v] : rule.m_vars) { std::cout << "\t\t" << k << " = " << v << ",\n"; }
    std::cout << "\t}\n";
}

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
        std::cout << term::red << term::bold << "<-[" << e.rule << "]- " << term::reset;

        for (const auto &i : e.in) { std::cout << i << " "; }
        std::cout << "\n";

        for (const auto &[k, v] : e.vars) { std::cout << "\t" << k << " = " << v << "\n"; }
    }
}
