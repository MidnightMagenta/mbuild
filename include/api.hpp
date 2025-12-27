#ifndef MBUILD_API_H
#define MBUILD_API_H

#include <build_graph.hpp>
#include <lua.hpp>

namespace mb::api {
namespace lua {
    // lua signature: build(rule : string, inputs : array, outputs : array, vars : optional table)
    int build(lua_State *L);

    // lua signature: rule(name : string, properties table)
    int rule(lua_State *L);

    // lua signature: set(key : string, value : string)
    int set(lua_State *L);

    // lua signature: get(key : string) -> value : string
    int get(lua_State *L);
}// namespace lua
}// namespace mb::api

#endif
