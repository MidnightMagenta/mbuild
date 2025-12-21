#ifndef MBUILD_API_H
#define MBUILD_API_H

#include <build_graph.hpp>
#include <filesystem>

namespace mb::api {
namespace fs = std::filesystem;

inline mb::BuildGraph g_buildGraph;

void source(const fs::path &path);

}// namespace mb::api

#endif
