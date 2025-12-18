#ifndef MBUILD_GRAPH_H
#define MBUILD_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <emit.hpp>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace mb {
namespace fs = std::filesystem;
namespace bs = boost;

class BuildGraph {
public:
    struct Rule {
        std::string name;
        std::string command;
        std::string depFile;
        std::string description;
    };

    struct Node {
        fs::path                   m_path;
        std::optional<std::string> m_rule;
    };

    using Graph  = bs::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Node>;
    using Vertex = bs::graph_traits<Graph>::vertex_descriptor;

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

    Vertex artifact(const fs::path &path);
    void   depends(const std::string &rule, const fs::path &depn, const std::vector<fs::path> &deps);
    void   depends(const std::string &rule, const Vertex depn, const std::vector<Vertex> &deps);
    void   emit(Emitter &e);

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
