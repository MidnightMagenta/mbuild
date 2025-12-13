cmake_minimum_required(VERSION 3.10)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(PROJECT_NAME "mbuild")
set(CMAKE_INSTALL_PREFIX /usr/local)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME})

find_package(Lua REQUIRED)

add_executable(mbuild
    src/main.cc
)

target_include_directories(mbuild PRIVATE include ${Lua_INCLUDE_DIRS})
target_link_libraries(mbuild PRIVATE ${Lua_LIBRARIES})

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
