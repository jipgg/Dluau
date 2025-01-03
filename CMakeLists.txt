cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(halua LANGUAGES CXX C)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(luau-dll)

file (GLOB_RECURSE STD_SOURCE ${CMAKE_SOURCE_DIR}/std/*.cpp)
add_library(halua-std SHARED ${STD_SOURCE})
target_compile_definitions(halua-std PRIVATE HALUA_API_EXPORT)
target_link_libraries(halua-std PRIVATE Luau.VM)

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
    halua-std
)
