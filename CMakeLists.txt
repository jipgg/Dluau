cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(minluau LANGUAGES CXX C)
set(LUAU_DIR luau-dll)
set(CLI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/cli)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

file (GLOB_RECURSE MINLUAU_SOURCE ${CMAKE_SOURCE_DIR}/minluau/*.cpp)
file(GLOB_RECURSE CLI_SOURCE ${CLI_SOURCE_DIR}/*.cpp)
file (GLOB_RECURSE MODULES_SOURCE ${CMAKE_SOURCE_DIR}/modules/*.cpp)

add_subdirectory(luau-dll)
add_library(minluau SHARED ${MINLUAU_SOURCE})
add_executable(minluau.cli WIN32 ${CLI_SOURCE})
add_library(minluau.modules SHARED ${MODULES_SOURCE})

target_compile_definitions(Luau.VM PRIVATE "LUA_API=extern \"C\" __declspec(dllexport)")
target_compile_definitions(Luau.Compiler PRIVATE "LUACODE_API=extern \"C\" __declspec(dllexport)")
target_compile_definitions(Luau.CodeGen PRIVATE "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
target_compile_definitions(minluau PUBLIC "LUA_API=extern \"C\" __declspec(dllimport)")
target_compile_definitions(minluau PUBLIC "LUACODE_API=extern \"C\" __declspec(dllimport)")
target_compile_definitions(minluau PUBLIC "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")
target_compile_definitions(minluau PRIVATE "LUA_API=extern \"C\" __declspec(dllexport)")
target_compile_definitions(minluau PRIVATE "LUACODE_API=extern \"C\" __declspec(dllexport)")
target_compile_definitions(minluau PRIVATE "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")

target_compile_definitions(minluau PRIVATE BUILD_LUA_DLL)
target_include_directories(minluau PRIVATE minluau)
target_include_directories(minluau PUBLIC minluau/include)
target_compile_definitions(minluau PRIVATE MINLUAU_API_EXPORT)
target_link_libraries(minluau PUBLIC Luau.VM Luau.Compiler Luau.CodeGen)

target_link_libraries(minluau.cli PRIVATE minluau Luau.Config)
set_target_properties(minluau.cli PROPERTIES OUTPUT_NAME minluau)

target_compile_definitions(minluau.modules PRIVATE MINLUAU_MODULES_API_EXPORT)
target_link_libraries(minluau.modules PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    minluau
)

