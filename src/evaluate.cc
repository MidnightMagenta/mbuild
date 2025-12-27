#include <api.hpp>
#include <lua.hpp>

void evaluate(const char *root) {
    std::filesystem::path rootDir(root);
    lua_State            *L = luaL_newstate();

    luaL_openlibs(L);
    lua_register(L, "build", mb::api::lua::build);
    lua_register(L, "rule", mb::api::lua::rule);

    if (luaL_dofile(L, (rootDir / "build.lua").string().c_str()) != LUA_OK) {
        std::string e = lua_tostring(L, -1);
        lua_close(L);
        throw std::runtime_error("LUA: " + e);
    }

    lua_close(L);
}
