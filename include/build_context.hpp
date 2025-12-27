#ifndef MBUILD_CONTEXT_H
#define MBUILD_CONTEXT_H

#include <build_graph.hpp>
#include <build_rules.hpp>
#include <string>
#include <vector>

namespace mb {
class BuildContext {
public:
    BuildContext() {}
    ~BuildContext() {}

    inline void build(const std::string                                  &rule,
                      const std::vector<std::string>                     &ins,
                      const std::vector<std::string>                     &outs,
                      const std::unordered_map<std::string, std::string> &vars) {
        build(rule, to_paths(ins), to_paths(outs), vars);
    }

    inline void build(const std::string                                  &rule,
                      const std::vector<fs::path>                        &ins,
                      const std::vector<fs::path>                         outs,
                      const std::unordered_map<std::string, std::string> &vars) {
        m_graph.build(rule).set_vars(vars).inputs(ins).outputs(outs, true);
    }

    void set_var(const std::string &key, const std::string &value) {
        m_vars[key] = value;
    }

    std::optional<std::string> get_var(const std::string &key) {
        auto it = m_vars.find(key);
        if (it == m_vars.end()) { return std::nullopt; }
        return it->second;
    }

    void emit(Emitter &e);

    inline BuildGraph &graph() {
        return m_graph;
    }

    inline BuildRules &rules() {
        return m_rules;
    }

private:
    std::vector<fs::path> to_paths(const std::vector<std::string> &p);

private:
    BuildGraph                                   m_graph;
    BuildRules                                   m_rules;
    std::unordered_map<std::string, std::string> m_vars;
};

inline BuildContext g_buildContext;
}// namespace mb

#endif
