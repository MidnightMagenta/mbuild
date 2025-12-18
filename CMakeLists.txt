cmake_minimum_required(VERSION 3.10)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(PROJECT_NAME "mbuild")
set(CMAKE_INSTALL_PREFIX /usr/local)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

project(${PROJECT_NAME})

find_package(Lua REQUIRED)
find_package(Boost REQUIRED)

add_executable(mbuild
    src/main.cc
    src/build_graph.cc

    include/build_graph.hpp
    include/emit.hpp
    include/dbg_emitter.hpp
)

target_include_directories(mbuild PRIVATE
    include
    ${Lua_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(mbuild PRIVATE
    ${Lua_LIBRARIES}
    ${Boost_LIBRARIES}
)

# install stuff
install(
    TARGETS mbuild
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/lua/
    DESTINATION share/${PROJECT_NAME}/lua
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/lua_stubs/
    DESTINATION share/${PROJECT_NAME}/lua_stubs
)
