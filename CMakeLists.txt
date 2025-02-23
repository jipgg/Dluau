cmake_minimum_required(VERSION 3.22.2)

project(Dluau LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(Dluau.Core.VERSION_MAJOR 0)
set(Dluau.Core.VERSION_MINOR 0)
set(Dluau.Core.VERSION_PATCH 1)

# Dependencies
find_package(Boost REQUIRED COMPONENTS container process filesystem)
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/nlohmann_json)
add_subdirectory(luau)
add_subdirectory(extern/dyncall-1.4)
set(DynLoad_DIR ${DynCall_SOURCE_DIR}/dynload)
set(DynCall_DIR ${DynCall_SOURCE_DIR}/dyncall)
set(DynCallback_DIR ${DynCall_SOURCE_DIR}/dyncallback)

# Add subdirectories
add_subdirectory(Core)
add_subdirectory(CLI)
add_subdirectory(Std)
