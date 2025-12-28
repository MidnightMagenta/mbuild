#ifndef MBUILD_GRAPH_H
#define MBUILD_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <emit.hpp>
#include <filesystem>
#include <span>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mb {
namespace fs = std::filesystem;

struct ArtifactID;
struct ActionID;

class ActionBuilder;

class BuildGraph {
public:
    struct Artifact {
        fs::path path;
    };

    struct Action {
        std::string                                  rule;
        std::unordered_map<std::string, std::string> vars;
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

public:
    BuildGraph() = delete;
    BuildGraph(const fs::path &root, const fs::path &build)
        : m_root(root),
          m_buildDir(build) {}
    ~BuildGraph() {}

    ArtifactID file(const fs::path &p);
    ActionID   action(const std::string &rule);
    void       action_var(ActionID a, const std::string &key, const std::string &value);
    void       consumes(ActionID a, ArtifactID in);
    void       produces(ActionID a, ArtifactID out);

    ActionBuilder build(const std::string &rule);

    void emit(Emitter &e);

private:
    inline fs::path normalize(const std::filesystem::path &p) {
        return (p.is_absolute() ? p.lexically_relative(m_root) : p).lexically_normal();
    }

    void validate();

private:
    const fs::path                      &m_root;
    const fs::path                      &m_buildDir;
    Graph                                m_graph;
    std::unordered_map<fs::path, Vertex> m_artifactIDMap;
};

struct ArtifactID {
    mb::BuildGraph::Vertex v;
};

struct ActionID {
    mb::BuildGraph::Vertex v;
};

class ActionBuilder {
public:
    ActionBuilder(BuildGraph &g, std::string rule)
        : m_graph(g),
          m_rule(std::move(rule)) {}

    inline ActionBuilder &inputs(std::initializer_list<fs::path> paths) {
        return inputs(std::span{paths.begin(), paths.size()});
    }

    ActionBuilder &inputs(std::span<const fs::path> ins);
    ActionBuilder &input(const fs::path &in);

    inline ActionBuilder &output(const fs::path &p, bool finish = false) {
        return outputs(std::span{&p, 1}, finish);
    }

    inline ActionBuilder &outputs(std::initializer_list<fs::path> paths, bool finish = false) {
        return outputs(std::span{paths.begin(), paths.size()}, finish);
    }

    ActionBuilder &outputs(std::span<const fs::path> out, bool finish = false);

    inline ActionBuilder &set_var(const std::string &key, const std::string &value) {
        m_vars[key] = value;
        return *this;
    }

    inline ActionBuilder &set_vars(const std::unordered_map<std::string, std::string> &vars) {
        for (const auto &[k, v] : vars) { set_var(k, v); }
        return *this;
    }

    void finalize();

    ActionID id() {
        return {m_action};
    }

private:
    mb::BuildGraph                              &m_graph;
    std::string                                  m_rule;
    std::vector<ArtifactID>                      m_inputs;
    std::vector<ArtifactID>                      m_outputs;
    std::unordered_map<std::string, std::string> m_vars;
    ActionID                                     m_action;
    bool                                         m_finalized = false;
};
}// namespace mb

#endif
