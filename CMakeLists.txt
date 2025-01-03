cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(halua LANGUAGES CXX C)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(luau-dll)

file (GLOB_RECURSE FILESYSTEM_SOURCE ${CMAKE_SOURCE_DIR}/std/*.cpp)
add_library(halua.filesystem SHARED ${FILESYSTEM_SOURCE})
target_compile_definitions(halua.filesystem PRIVATE BUILD_HALUA_FILESYSTEM_DLL)
target_link_libraries(halua.filesystem PRIVATE Luau.VM)

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)
if (LINUX)
    add_executable(halua ${SOURCE_FILES})
elseif(WIN32)
    add_executable(halua WIN32 ${SOURCE_FILES})
endif()
target_link_libraries(halua PRIVATE
    Luau.VM
    Luau.Compiler
    Luau.Config
    halua.filesystem
)
