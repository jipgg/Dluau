cmake_minimum_required(VERSION 3.22.2)

project(dluau LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Dependencies
find_package(Boost REQUIRED COMPONENTS container)
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/nlohmann_json)
add_subdirectory(luau)


# Add subdirectories
add_subdirectory(Core)
add_subdirectory(CLI)
