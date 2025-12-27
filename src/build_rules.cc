#include <build_rules.hpp>
#include <emit.hpp>
#include <stdexcept>

// mb::BuilrRUles::Rule definitions
using Rule = mb::BuildRules::Rule;

Rule &Rule::append_var(const std::string &key, const std::string &value) {
    auto it = m_vars.find(key);
    if (it == m_vars.end()) { m_vars[key] = value; }
    it->second.append(value);
    return *this;
}

std::string Rule::get_var(const std::string &key) {
    auto it = m_vars.find(key);
    if (it == m_vars.end()) { return std::string(); }
    return it->second;
}

// mb::BuildRules definitions
mb::BuildRules::Rule *mb::BuildRules::create(const std::string &name) {
    auto [it, inserted] = m_rules.try_emplace(name, name);
    if (!inserted) { throw std::runtime_error("Rule already exists: " + name); }
    return &it->second;
}

mb::BuildRules::Rule *mb::BuildRules::get(const std::string &name) {
    auto it = m_rules.find(name);
    if (it == m_rules.end()) { return nullptr; }
    return &it->second;
}

void mb::BuildRules::emit(mb::Emitter &e) {
    for (const auto [k, v] : m_rules) { e.rule(v); }
}
