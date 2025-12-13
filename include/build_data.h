#ifndef MBUILD_DAG_H
#define MBUILD_DAG_H

#include <filesystem>
#include <optional>
#include <string>
#include <table.h>
#include <unordered_map>
#include <vector>

namespace mb {
struct Rule;
using RuleTable = mb::NamedTable<std::string, Rule>;
using RuleID    = mb::NamedTable<std::string, Rule>::ID;

struct Source;
using SourceTable = mb::NamedTable<std::filesystem::path, Source>;
using SourceID    = mb::NamedTable<std::filesystem::path, Source>::ID;

struct Module;
using ModuleTable = mb::NamedTable<std::string, Module>;
using ModuleID    = mb::NamedTable<std::string, Module>::ID;

struct Target;
using TargetTable = mb::NamedTable<std::string, Target>;
using TargetID    = mb::NamedTable<std::string, Target>::ID;

struct Flags {
    std::vector<std::string> flags;
    bool                     override = false;// override or append
};

struct Rule {
    std::string name;
    std::string command;
    std::string deps_file;
    std::string deps;
    std::string description;

    enum class Kind {
        Compile,
        ModuleLink,
        TargetLink,
    } kind;

    std::vector<std::string> fileAssociations;
};

struct Source {
    std::string              input;
    std::string              output;
    std::optional<RuleID>    rule;
    std::vector<std::string> compileFlags;
};

struct Module {
    std::string                            name;
    std::vector<SourceID>                  sources;
    std::optional<RuleID>                  rule;
    std::unordered_map<std::string, Flags> flags;
};

struct Target {
    std::string                            name;
    std::vector<ModuleID>                  modules;
    std::optional<RuleID>                  rule;
    std::unordered_map<std::string, Flags> flags;
};

class Project {
public:
    Project() {}
    ~Project() {}

    inline SourceTable       &sources() { return m_sources; }
    inline const SourceTable &sources() const { return m_sources; }

    inline ModuleTable       &modules() { return m_modules; }
    inline const ModuleTable &modules() const { return m_modules; }

    inline TargetTable       &targets() { return m_targets; }
    inline const TargetTable &targets() const { return m_targets; }

    inline void set_var(const std::string &name, const std::string &value) { m_vars[name] = value; }

    // rules
    inline void        add_rule(const std::string &name) { m_rules.insert(name, Rule{.name = name}); }
    inline Rule       *get_rule(const std::string &name) { return m_rules.get(name); }
    inline const Rule *get_rule(const std::string &name) const { return m_rules.get(name); }

    // sources
    inline void add_source(const std::filesystem::path &p) {
        auto source_path = std::filesystem::weakly_canonical(p);
        m_sources.insert(source_path, Source{.input = source_path});
    }

    inline Source *get_source(const std::filesystem::path &p) {
        return m_sources.get(std::filesystem::weakly_canonical(p));
    }
    inline const Source *get_source(const std::filesystem::path &p) const {
        return m_sources.get(std::filesystem::weakly_canonical(p));
    }

    // modules
    inline void          add_module(const std::string &name) { m_modules.insert(name, Module{.name = name}); }
    inline Module       *get_module(const std::string &name) { return m_modules.get(name); }
    inline const Module *get_module(const std::string &name) const { return m_modules.get(name); }

    // targets
    inline void          add_target(const std::string &name) { m_targets.insert(name, Target{.name = name}); }
    inline Target       *get_target(const std::string &name) { return m_targets.get(name); }
    inline const Target *get_target(const std::string &name) const { return m_targets.get(name); }

private:
    SourceTable m_sources;
    ModuleTable m_modules;
    TargetTable m_targets;

    RuleTable m_rules;

    std::unordered_map<std::string, std::vector<std::string>> m_projectFlags;
    std::unordered_map<std::string, std::string>              m_vars;
};
}// namespace mb

#endif
