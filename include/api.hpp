#ifndef MBUILD_API_H
#define MBUILD_API_H

#include <build_graph.hpp>
#include <lua.hpp>

namespace mb::api {
namespace lua {
    // lua signature: build(rule (string), inputs (array), outputs(array))
    int build(lua_State *L);
}// namespace lua
}// namespace mb::api

#endif
