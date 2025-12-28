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

    // lua signature build_dir(path : string)
    int build_dir(lua_State *L);

    // lua signature: get_rule(extention : string) -> rule : string
    int get_rule(lua_State *L);

    namespace fs {
        int extension(lua_State *L);
        int normalize(lua_State *L);
    }// namespace fs
}// namespace lua
}// namespace mb::api

#endif
