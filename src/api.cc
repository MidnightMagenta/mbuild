#include <api.hpp>

void mb::api::source(const fs::path &path) {
    return;
    // g_buildGraph.artifact(path);
    //
    // fs::path compileArtifact = path;
    // compileArtifact.replace_extension(".o");
    //
    // g_buildGraph.artifact(compileArtifact);
    //
    // std::string rule = g_buildGraph.get_rule_for_file(path.extension()).value_or("");
    // if (rule.empty()) { throw std::runtime_error("Could not find build rule for source " + path.string()); }
    //
    // g_buildGraph.depends(rule, compileArtifact, path);
}
