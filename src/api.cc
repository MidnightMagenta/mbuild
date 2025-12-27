#include <api.hpp>
#include <build_context.hpp>
#include <iostream>
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
    // expected LUA stack layout
    // 1 -> rule        (string)
    // 2 -> inputs      (table : array)
    // 3 -> outputs     (table : array)
    // 4 -> vars        (table : array) [optional]

    lua_Number sTop = lua_gettop(L);
    if (!(sTop == 3 || sTop == 4)) { luaL_error(L, "build() expects 3 or 4 arguments. Got %d", sTop); }

    // retreive the rule
    const char *rule = luaL_checkstring(L, 1);

    // verify the other two parameters
    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    if (sTop == 4) { luaL_checktype(L, 4, LUA_TTABLE); }

    // ---- no lua_error() throwing calls from this line forward ----

    try {
        std::vector<std::string> ins  = read_str_table(L, 2);
        std::vector<std::string> outs = read_str_table(L, 3);
        if (sTop == 4) { std::cout << "Ignoring 'vars'. Unimplemented\n"; }
        g_buildContext.build(rule, ins, outs);
    } catch (const std::runtime_error &e) { luaL_error(L, e.what()); } catch (...) {
        luaL_error(L, "Unknown error");
    }

    return 0;
}

int mb::api::lua::rule(lua_State *L) {
    // expected LUA stack layout
    // 1 -> rule name       (string)
    // 2 -> property table  (table : map)

    if (lua_gettop(L) != 2) { luaL_error(L, "rule() expects exactly two arguments"); }
    if (!lua_isstring(L, 1) || !lua_istable(L, 2)) {
        luaL_error(L, "Invalid argument types: rule() signature is rule(name : string, properties : table)");
    }

    // ---- no lua_error() throwing calls from this line forward ----

    try {
        mb::BuildRules::Rule *r = g_buildContext.rules().create(lua_tostring(L, 1));
        if (r == nullptr) { throw std::runtime_error("Failed to create a rule"); }

        lua_pushnil(L);
        while (lua_next(L, 2)) {// 2 -> the property table
            // current stack
            // -2 -> key (string)
            // -1 -> value

            if (!lua_isstring(L, -2) || !lua_isstring(L, -1)) {
                throw std::runtime_error("rule() table indices and values must be of type `string`");
            }
            const char *key = lua_tostring(L, -2);
            if (!r->get_var(key).empty()) {
                throw std::runtime_error("Rule property `" + std::string(key) + "` was set twice");
            }
            const char *value = lua_tostring(L, -1);
            r->set_var(key, value);

            lua_pop(L, 1);
        }
    } catch (const std::runtime_error &e) { luaL_error(L, e.what()); } catch (...) {
        luaL_error(L, "Unknown error");
    }

    return 0;
}
