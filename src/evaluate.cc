#include <api.hpp>
#include <filesystem>
#include <lua.hpp>
#include <string>

namespace lua = mb::api::lua;

static const luaL_Reg mbuild_lib[] = {
        {"build", lua::build},
        {"rule", lua::rule},
        {"set", lua::set},
        {"get", lua::get},
        {"build_dir", lua::build_dir},
        {"get_rule", lua::get_rule},
        {nullptr, nullptr},
};

static const luaL_Reg fs_lib[] = {
        {"extension", lua::fs::extension},
        {"normalize", lua::fs::normalize},
        {nullptr, nullptr},
};

#define register_lib(ls, n, lib)                                                                                       \
    luaL_newlib(ls, lib);                                                                                              \
    lua_setglobal(ls, n);

void add_lua_path(lua_State *L, const char *path) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");

    std::string newPath = lua_tostring(L, -1);
    newPath += ";";
    newPath += path;

    lua_pop(L, 1);
    lua_pushlstring(L, newPath.data(), newPath.size());
    lua_setfield(L, -2, "path");

    lua_pop(L, 1);
}

void init_mbuild(lua_State *L) {
    luaL_openlibs(L);

    register_lib(L, "mbuild", mbuild_lib);
    register_lib(L, "fs", fs_lib);

    add_lua_path(L, "/usr/local/share/mbuild/lua/?.lua");
    add_lua_path(L, "/usr/local/share/mbuild/lua/?/init.lua");
    add_lua_path(L, (std::filesystem::current_path() / "lua/?.lua").string().c_str());
    add_lua_path(L, (std::filesystem::current_path() / "lua/?/init.lua").string().c_str());
    add_lua_path(L, (std::filesystem::current_path() / "scripts/lua/?.lua").string().c_str());
    add_lua_path(L, (std::filesystem::current_path() / "scripts/lua/?/init.lua").string().c_str());
    add_lua_path(L, (std::filesystem::current_path() / "scripts/mbuild/lua/?.lua").string().c_str());
    add_lua_path(L, (std::filesystem::current_path() / "scripts/mbuild/lua/?/init.lua").string().c_str());
}

void evaluate(const char *root) {
    std::filesystem::path rootDir(root);
    lua_State            *L = luaL_newstate();

    init_mbuild(L);

    if (luaL_dofile(L, (rootDir / "build.lua").string().c_str()) != LUA_OK) {
        std::string e = lua_tostring(L, -1);
        lua_close(L);
        throw std::runtime_error("LUA: " + e);
    }

    lua_close(L);
}
