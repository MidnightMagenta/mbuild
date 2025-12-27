#ifndef MBUILD_RULE_H
#define MBUILD_RULE_H

#include <optional>
#include <string>
#include <unordered_map>

namespace mb {
class Emitter;

class BuildRules {
public:
    struct Rule {
        Rule(const std::string &name)
            : m_name(name) {}

        const std::string                            m_name;
        std::unordered_map<std::string, std::string> m_vars;

        inline Rule &set_var(const std::string &key, const std::string &value) {
            m_vars[key] = value;
            return *this;
        }
        Rule                      &append_var(const std::string &key, const std::string &value);
        std::optional<std::string> get_var(const std::string &key);
        inline void                delete_var(const std::string &key) {
            m_vars.erase(key);
        }
    };

    Rule *create(const std::string &name);
    Rule *get(const std::string &name);

    void emit(Emitter &e);

public:
    BuildRules() {}
    ~BuildRules() {}

private:
    std::unordered_map<std::string, Rule> m_rules;
};
}// namespace mb

#endif
