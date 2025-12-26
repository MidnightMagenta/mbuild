#include <api.hpp>
#include <build_context.hpp>
#include <vector>

// all lua_error() throwing calls may only be used in the API boundary function.
// All fuunctions that are not called directly from LUA must not throw a lua_error()

static std::vector<std::string> read_str_table(lua_State *L, int idx) {
    std::vector<std::string> res;

    idx = lua_absindex(L, idx);

    lua_Integer len = luaL_len(L, idx);
    res.reserve(len);
    for (lua_Integer i = 1; i <= len; ++i) {
        lua_geti(L, idx, i);

        // value from the table is now at -1 on the stack
        if (!lua_isstring(L, -1)) {
            throw std::runtime_error("Expected string at index " + std::to_string(i) + " in array");
        }
        res.push_back(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    return res;
}

int mb::api::lua::build(lua_State *L) {
    // expected LUA stack layout upon function entry
    // 1 -> rule        (string)
    // 2 -> inputs      (table : array)
    // 3 -> outputs     (table : array)

    if (lua_gettop(L) != 3) { luaL_error(L, "build() expects exactly 3 arguments"); }

    // retreive the rule
    const char *rule = luaL_checkstring(L, 1);

    // verify the other two parameters
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    // ---- no lua_error() throwing calls from this line forward ----

    try {
        std::vector<std::string> ins  = read_str_table(L, 2);
        std::vector<std::string> outs = read_str_table(L, 3);
        g_buildContext.build(rule, ins, outs);
    } catch (const std::runtime_error &e) { luaL_error(L, e.what()); }

    return 0;
}
