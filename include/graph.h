#ifndef MBUILD_GRAPH_H
#define MBUILD_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/topological_sort.hpp>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <iostream>

namespace mb {
class DepGraph {
public:
    struct Artifact {
        std::filesystem::path      path;
        std::optional<std::string> rule;
    };

    using Graph  = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Artifact>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

public:
    DepGraph() {}
    ~DepGraph() {}

    void artifact(const std::filesystem::path &path) {
        std::filesystem::path canon_path = normalize(path);
        if (m_vertexMap.contains(canon_path)) { return; }
        auto v = boost::add_vertex(m_graph);

        m_graph[v].path = canon_path;
        m_vertexMap.emplace(canon_path, v);
    }

    void depends(const std::string                        &rule,
                 const std::filesystem::path              &dependent,
                 const std::vector<std::filesystem::path> &dependencies) {
        std::filesystem::path canon_depn = normalize(dependent);

        auto it = m_vertexMap.find(canon_depn);
        if (it == m_vertexMap.end()) { throw std::runtime_error("Attempted to link vertices that do not exist"); }
        Vertex depn = it->second;

        std::vector<Vertex> deps;
        for (const auto &d : dependencies) {
            std::filesystem::path canon_dep = normalize(d);

            it = m_vertexMap.find(canon_dep);
            if (it == m_vertexMap.end()) { throw std::runtime_error("Attempted to link vertices that do not exist"); }
            deps.push_back(it->second);
        }

        depends(rule, depn, deps);
    }

    void validate() {
        std::vector<Vertex> order;
        try {
            boost::topological_sort(m_graph, std::back_inserter(order));
        } catch (const boost::not_a_dag &) { throw std::runtime_error("Cyclic"); }
    }

    void print() {
        for (auto v : boost::make_iterator_range(vertices(m_graph))) {
            if (!m_graph[v].rule) continue;

            std::vector<std::string> inputs;
            for (auto e : boost::make_iterator_range(boost::in_edges(v, m_graph))) {
                inputs.push_back(m_graph[source(e, m_graph)].path.string());
            }
            std::cout << "build " << m_graph[v].path.string() << ": " << m_graph[v].rule.value() << " ";
            for (const auto i : inputs) { std::cout << i << " "; }

            std::cout << "\n";
        }
    }

private:
    std::filesystem::path normalize(const std::filesystem::path &p) {
        auto rel = std::filesystem::absolute(p);
        return rel.lexically_normal();
    }

    void depends(const std::string &rule, Vertex depn, const std::vector<Vertex> &deps) {
        m_graph[depn].rule = rule;
        for (const auto d : deps) { auto [e, i] = boost::add_edge(d, depn, m_graph); }
    }

private:
    Graph                                             m_graph;
    std::unordered_map<std::filesystem::path, Vertex> m_vertexMap;
};
}// namespace mb

#endif
