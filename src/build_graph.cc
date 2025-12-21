#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/topological_sort.hpp>
#include <build_graph.hpp>

// invariant assumptions:
//  - an artifact can have 0 or 1 producing action nodes
//  - all artifact paths are canonical relative to m_root
//  - an action node can have exactly one out edge
//  - the graph is acyclic before emission
//  - nodes of same type may not have edges between eachother

// Inserts an artifact with the specified path
mb::BuildGraph::Vertex mb::BuildGraph::artifact(const fs::path &path) {
    fs::path canonPath = normalize(path);
    if (m_vertexIDMap.contains(canonPath)) { return m_vertexIDMap.find(canonPath)->second; }

    auto v          = boost::add_vertex(m_graph);
    m_graph[v].type = Node::Type::Artifact;
    m_graph[v].data = Artifact{.path = canonPath};
    m_vertexIDMap.emplace(canonPath, v);
    return v;
}

// inserts a dependency between two artifacts referenced by their path
void mb::BuildGraph::depends(const std::string &rule, const fs::path &dependent, const fs::path &dependency) {
    fs::path canonDependent  = normalize(dependent);
    fs::path canonDependency = normalize(dependency);

    auto dependentID = m_vertexIDMap.find(canonDependent);
    if (dependentID == m_vertexIDMap.end()) {
        throw std::runtime_error("Can not assign dependency to non existant artifact " + canonDependent.string());
    }
    auto dependencyID = m_vertexIDMap.find(canonDependency);
    if (dependencyID == m_vertexIDMap.end()) {
        throw std::runtime_error("Cannot assign non existant artifact " + canonDependency.string() +
                                 " as a dependency");
    }

    depends(rule, dependentID->second, dependencyID->second);
}

// inserts a dependency between an array of artifacts referenced by their path
void mb::BuildGraph::depends_a(const std::string           &rule,
                               const fs::path              &dependent,
                               const std::vector<fs::path> &dependencies) {
    for (const auto &d : dependencies) { depends(rule, dependent, d); }
}

// inserts a dependecy between two artifacts referenced by their vertex ID
void mb::BuildGraph::depends(const std::string &rule, const Vertex dependent, const Vertex dependency) {
    assert(dependent != dependency);
    assert(m_graph[dependent].type == Node::Type::Artifact);
    assert(m_graph[dependency].type == Node::Type::Artifact);

    DegreeSize degree = boost::in_degree(dependent, m_graph);
    assert(degree == 0 || degree == 1);

    if (degree == 0) {
        Vertex v        = boost::add_vertex(m_graph);
        m_graph[v].type = Node::Type::Action;
        m_graph[v].data = Action{.rule = rule};
        boost::add_edge(v, dependent, m_graph);
    }

    auto in = boost::in_edges(dependent, m_graph);
    if (in.first == in.second) { throw std::runtime_error("No producting action"); }
    auto  action_v = source(*in.first, m_graph);
    auto &node     = m_graph[action_v];
    assert(node.type == Node::Type::Action);
    if (std::get<Action>(node.data).rule != rule) { throw std::runtime_error("Conflicting rules for artifact"); }

    assert(!boost::edge(dependency, action_v, m_graph).second);
    boost::add_edge(dependency, action_v, m_graph);
}

// inserts a dependency between and array of artifacts referenced by their vertex ID
void mb::BuildGraph::depends_a(const std::string         &rule,
                               const Vertex               dependent,
                               const std::vector<Vertex> &dependencies) {
    for (const auto &d : dependencies) { depends(rule, dependent, d); }
}

// Rule related API

void mb::BuildGraph::define_rule(Rule rule) {
    auto [it, inserted] = m_rules.emplace(rule.name, std::move(rule));
    if (!inserted) { throw std::runtime_error("Rule redefinition: " + rule.name); }
}

const mb::Rule &mb::BuildGraph::get_rule(const std::string &name) const {
    auto it = m_rules.find(name);
    if (it == m_rules.end()) { throw std::runtime_error("Unknown rule: " + name); }
    return it->second;
}

mb::Rule &mb::BuildGraph::require_rule(const std::string &rule) {
    auto it = m_rules.find(rule);
    if (it == m_rules.end()) { throw std::runtime_error("Unknown rule: " + rule); }
    return it->second;
}

// Rule variable related API

void mb::BuildGraph::set_rule_var(const std::string &rule, const std::string &name, const std::string &value) {
    require_rule(rule).vars[name] = value;
}

void mb::BuildGraph::append_rule_var(const std::string &rule, const std::string &name, const std::string &value) {
    auto &vars = require_rule(rule).vars;
    auto  it   = vars.find(name);

    if (it == vars.end()) {
        vars[name] = value;
    } else {
        it->second.append(" ");
        it->second.append(value);
    }
}

std::optional<std::string> mb::BuildGraph::get_rule_var(const std::string &rule, const std::string &name) {
    Rule &r  = require_rule(rule);
    auto  it = r.vars.find(name);
    if (it != r.vars.end()) { return it->second; }

    return std::nullopt;
}

// ! Rule related API

// Global variable related API

void mb::BuildGraph::set_var(const std::string &name, const std::string &value) {
    m_vars[name] = value;
}

void mb::BuildGraph::set_rule_association(const std::string &rule, const std::vector<std::string> &exts) {
    for (const auto &e : exts) { m_ruleFileAssociations[e] = rule; }
}

std::optional<std::string> mb::BuildGraph::get_rule_for_file(const std::string &ext) {
    auto it = m_ruleFileAssociations.find(ext);
    if (it != m_ruleFileAssociations.end()) { return it->second; }
    return std::nullopt;
}

void mb::BuildGraph::append_var(const std::string &name, const std::string &value) {
    auto it = m_vars.find(name);
    if (it == m_vars.end()) {
        m_vars[name] = value;
        return;
    }
    it->second.append(" " + value);
}

std::optional<std::string> mb::BuildGraph::get_var(const std::string &name) const {
    auto it = m_vars.find(name);
    if (it != m_vars.end()) { return it->second; }
    return std::nullopt;
}

// ! Global variable related API

// emits the graph via a given emitter
void mb::BuildGraph::emit(Emitter &e) {
    validate();
    e.begin();

    for (const auto &[k, v] : m_vars) { e.global_var(k, v); }
    for (const auto &[k, v] : m_rules) { e.rule(v); }

    for (auto v : boost::make_iterator_range(vertices(m_graph))) {
        if (m_graph[v].type != Node::Type::Action) { continue; }

        std::string rule = std::get<Action>(m_graph[v].data).rule;

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

        e.edge(rule, targets, deps);
    }

    e.end();
}

// validates that the graph is consistent with given invariants
void mb::BuildGraph::validate() {

    try {
        std::vector<Graph::vertex_descriptor> topo;
        boost::topological_sort(m_graph, std::back_inserter(topo));
    } catch (const boost::not_a_dag) { throw std::runtime_error("Circular dependencies"); }
}
