#ifndef MBUILD_GRAPH_H
#define MBUILD_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <emit.hpp>
#include <filesystem>
#include <string>
#include <types.hpp>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mb {
namespace fs = std::filesystem;

class BuildGraph {
public:
    struct Artifact {
        fs::path path;
    };
    struct Action {
        std::string rule;
    };

    struct Node {
        enum class Type {
            Artifact,
            Action,
        } type;

        std::variant<Artifact, Action> data;
    };

    using Graph      = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Node>;
    using Vertex     = boost::graph_traits<Graph>::vertex_descriptor;
    using DegreeSize = boost::graph_traits<Graph>::degree_size_type;

    using VarStorage = std::unordered_map<std::string, std::string>;

public:
    BuildGraph() {
        m_root = fs::current_path();
    }
    BuildGraph(const fs::path &root) {
        m_root = root;
    }
    ~BuildGraph() {}

    void set_root(const fs::path &root) {
        m_root = root;
    }
    const fs::path &get_root() const {
        return m_root;
    }

    Vertex file(const fs::path &path) {
        return artifact(path);
    }

    void build(const fs::path &out, const std::string &rule, const std::vector<fs::path> &ins) {
        depends_a(rule, out, ins);
    }

    void build(Vertex out, const std::string &rule, std::vector<Vertex> &ins) {
        depends_a(rule, out, ins);
    }

    void        define_rule(Rule rule);
    const Rule &get_rule(const std::string &name) const;
    void        set_rule_var(const std::string &rule, const std::string &name, const std::string &value);
    void        append_rule_var(const std::string &rule, const std::string &name, const std::string &value);
    std::optional<std::string> get_rule_var(const std::string &rule, const std::string &name);
    void                       set_rule_association(const std::string &rule, const std::vector<std::string> &exts);
    std::optional<std::string> get_rule_for_file(const std::string &ext);

    void                       set_var(const std::string &name, const std::string &value);
    void                       append_var(const std::string &name, const std::string &value);
    std::optional<std::string> get_var(const std::string &name) const;

    void emit(Emitter &e);

private:
    Vertex artifact(const fs::path &path);
    void   depends(const std::string &rule, const fs::path &dependent, const fs::path &dependency);
    void   depends_a(const std::string &rule, const fs::path &dependent, const std::vector<fs::path> &dependencies);
    void   depends(const std::string &rule, const Vertex dependent, const Vertex dependency);
    void   depends_a(const std::string &rule, const Vertex dependent, const std::vector<Vertex> &dependencies);

    Rule &require_rule(const std::string &rule);

    inline fs::path normalize(const std::filesystem::path &p) {
        return (p.is_absolute() ? p.lexically_relative(m_root) : p).lexically_normal();
    }

    void validate();

private:
    fs::path                             m_root;
    Graph                                m_graph;
    std::unordered_map<fs::path, Vertex> m_vertexIDMap;

    std::unordered_map<std::string, Rule>        m_rules;
    std::unordered_map<std::string, std::string> m_ruleFileAssociations;
    VarStorage                                   m_vars;
};
}// namespace mb

#endif
