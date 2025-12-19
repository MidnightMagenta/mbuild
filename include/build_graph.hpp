#ifndef MBUILD_GRAPH_H
#define MBUILD_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <emit.hpp>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mb {
namespace fs = std::filesystem;

class BuildGraph {
public:
    struct Rule {
        std::string name;
        std::string command;
        std::string depFile;
        std::string description;
    };

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

    void set_default_rules();

    fs::path artifact(const fs::path &path);
    void     depends(const std::string &rule, const fs::path &dependent, const fs::path &dependency);
    void     depends_a(const std::string &rule, const fs::path &dependent, const std::vector<fs::path> &dependencies);
    void     depends(const std::string &rule, const Vertex dependent, const Vertex dependency);
    void     depends_a(const std::string &rule, const Vertex dependent, const std::vector<Vertex> &dependencies);
    void     emit(Emitter &e);

private:
    inline fs::path normalize(const std::filesystem::path &p) {
        return (p.is_absolute() ? p.lexically_relative(m_root) : p).lexically_normal();
    }

private:
    fs::path                             m_root;
    Graph                                m_graph;
    std::unordered_map<fs::path, Vertex> m_vertexIDMap;
    std::vector<Rule>                    m_rules;
};
}// namespace mb

#endif
