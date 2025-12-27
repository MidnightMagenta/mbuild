#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/topological_sort.hpp>
#include <build_graph.hpp>

// invariant assumptions:
//  - an artifact can have 0 or 1 producing action nodes
//  - all artifact paths are canonical relative to m_root
//  - an action node can have exactly one out edge
//  - the graph is acyclic before emission
//  - nodes of same type may not have edges between eachother

// BuildGraph implementation

mb::ArtifactID mb::BuildGraph::file(const fs::path &p) {
    auto np = normalize(p);
    auto it = m_artifactIDMap.find(np);
    if (it != m_artifactIDMap.end()) { return {it->second}; }

    Vertex v   = boost::add_vertex(m_graph);
    m_graph[v] = Node{
            .type = Node::Type::Artifact,
            .data = Artifact{np},
    };

    m_artifactIDMap[np] = v;
    return {v};
}

mb::ActionID mb::BuildGraph::action(const std::string &rule) {
    Vertex v   = boost::add_vertex(m_graph);
    m_graph[v] = Node{
            .type = Node::Type::Action,
            .data = Action{rule},
    };
    return {v};
}

void mb::BuildGraph::action_var(ActionID a, const std::string &key, const std::string &value) {
    assert(m_graph[a.v].type == Node::Type::Action);
    std::get<Action>(m_graph[a.v].data).vars[key] = value;
}

void mb::BuildGraph::consumes(ActionID a, ArtifactID in) {
    boost::add_edge(in.v, a.v, m_graph);
}

void mb::BuildGraph::produces(ActionID a, ArtifactID out) {
    if (boost::in_degree(out.v, m_graph) != 0) {
        throw std::runtime_error("Artifact " + std::get<Artifact>(m_graph[out.v].data).path.string() +
                                 " already has a producer");
    }
    boost::add_edge(a.v, out.v, m_graph);
}

void mb::BuildGraph::emit(Emitter &e) {
    validate();
    for (auto v : boost::make_iterator_range(vertices(m_graph))) {
        if (m_graph[v].type != Node::Type::Action) { continue; }

        std::string                                  rule = std::get<Action>(m_graph[v].data).rule;
        std::unordered_map<std::string, std::string> vars = std::get<Action>(m_graph[v].data).vars;

        std::vector<std::string> targets;
        for (auto t_e : boost::make_iterator_range(out_edges(v, m_graph))) {
            Vertex artifact = target(t_e, m_graph);
            assert(m_graph[artifact].type == Node::Type::Artifact);
            targets.push_back(std::get<Artifact>(m_graph[artifact].data).path.string());
        }

        std::vector<std::string> deps;
        for (auto d_e : boost::make_iterator_range(in_edges(v, m_graph))) {
            Vertex artifact = source(d_e, m_graph);
            assert(m_graph[artifact].type == Node::Type::Artifact);
            deps.push_back(std::get<Artifact>(m_graph[artifact].data).path.string());
        }

        e.edge(rule, targets, deps, vars);
    }
}

// validates that the graph is consistent with given invariants
void mb::BuildGraph::validate() {

    try {
        std::vector<Graph::vertex_descriptor> topo;
        boost::topological_sort(m_graph, std::back_inserter(topo));
    } catch (const boost::not_a_dag) { throw std::runtime_error("Circular dependencies"); }
}

// ActionBuilder implementation

mb::ActionBuilder mb::BuildGraph::build(const std::string &rule) {
    return ActionBuilder(*this, rule);
}

mb::ActionBuilder &mb::ActionBuilder::inputs(std::span<const fs::path> ins) {
    if (m_finalized) { throw std::runtime_error("Can not add artifacts to a finalized action"); }
    for (auto &p : ins) { m_inputs.push_back(m_graph.file(p)); }
    return *this;
}

mb::ActionBuilder &mb::ActionBuilder::input(const fs::path &in) {
    if (m_finalized) { throw std::runtime_error("Can not add artifacts to a finalized action"); }
    m_inputs.push_back(m_graph.file(in));
    return *this;
}

mb::ActionBuilder &mb::ActionBuilder::outputs(std::span<const fs::path> out, bool finish) {
    if (m_finalized) { throw std::runtime_error("Can not add artifacts to a finalized action"); }
    for (auto &p : out) { m_outputs.push_back(m_graph.file(p)); }
    if (finish) { finalize(); }
    return *this;
}

void mb::ActionBuilder::finalize() {
    if (m_finalized) { throw std::runtime_error("Can not refinalize a finalized action"); }
    if (m_outputs.empty()) { throw std::runtime_error("Action has no outputs"); }

    m_action = m_graph.action(m_rule);
    for (auto &in : m_inputs) { m_graph.consumes(m_action, in); }
    for (auto &out : m_outputs) { m_graph.produces(m_action, out); }
    for (const auto &[k, v] : m_vars) { m_graph.action_var(m_action, k, v); }

    m_finalized = true;
}
