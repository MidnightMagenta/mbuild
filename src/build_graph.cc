#include <build_graph.hpp>

void mb::BuildGraph::set_default_rules() {}

mb::BuildGraph::Vertex mb::BuildGraph::artifact(const fs::path &path) {
    fs::path canonPath = normalize(path);
    if (m_vertexIDMap.contains(canonPath)) { return m_vertexIDMap.find(canonPath)->second; }

    auto v            = bs::add_vertex(m_graph);
    m_graph[v].m_path = canonPath;
    m_vertexIDMap.emplace(canonPath, v);
    return v;
}

void mb::BuildGraph::depends(const std::string &rule, const fs::path &depn, const std::vector<fs::path> &deps) {
    fs::path canonDependent = normalize(depn);
    auto     it             = m_vertexIDMap.find(canonDependent);
    if (it == m_vertexIDMap.end()) {
        throw std::runtime_error("Can't link non existent artifact " + canonDependent.string());
    }

    Vertex              depnID = it->second;
    std::vector<Vertex> depsID;
    for (const auto &d : deps) {
        fs::path canonDependency = normalize(d);
        it                       = m_vertexIDMap.find(canonDependency);
        if (it == m_vertexIDMap.end()) {
            throw std::runtime_error("Can't link non existent artifact " + canonDependency.string());
        }
        depsID.push_back(it->second);
    }

    depends(rule, depnID, depsID);
}

void mb::BuildGraph::depends(const std::string &rule, const Vertex depn, const std::vector<Vertex> &deps) {
    m_graph[depn].m_rule = rule;
    for (const auto d : deps) { auto [e, i] = bs::add_edge(d, depn, m_graph); }
}

void mb::BuildGraph::emit(Emitter &e) {
    e.begin();

    for (auto v : bs::make_iterator_range(vertices(m_graph))) {
        if (!m_graph[v].m_rule) continue;

        std::vector<std::string> inputs;
        for (auto e : bs::make_iterator_range(bs::in_edges(v, m_graph))) {
            inputs.push_back(m_graph[source(e, m_graph)].m_path.string());
        }

        e.edge(m_graph[v].m_rule.value(), m_graph[v].m_path, inputs);
    }

    e.end();
}
