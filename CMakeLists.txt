cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(minluau LANGUAGES CXX C)

set(CLI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/cli)
set(LUA_EXPORT "LUA_API=extern \"C\" __declspec(dllexport)")
set(LUACODE_EXPORT "LUACODE_API=extern \"C\" __declspec(dllexport)")
set(LUACODEGEN_EXPORT "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
set(LUA_IMPORT "LUA_API=extern \"C\" __declspec(dllimport)")
set(LUACODE_IMPORT "LUACODE_API=extern \"C\" __declspec(dllimport)")
set(LUACODEGEN_IMPORT "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

file(GLOB_RECURSE MINLUAU_SOURCE minluau/*.cpp)
file(GLOB_RECURSE CLI_SOURCE cli/*.cpp)
file(GLOB_RECURSE MODULES_SOURCE modules/*.cpp)

add_subdirectory(luau)
add_library(minluau SHARED ${MINLUAU_SOURCE})
add_executable(minluau.cli WIN32 ${CLI_SOURCE})
add_library(minluau.modules SHARED ${MODULES_SOURCE})

target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
target_compile_definitions(Luau.VM PRIVATE ${LUA_EXPORT})
target_compile_definitions(Luau.Compiler PRIVATE ${LUACODE_EXPORT})
target_compile_definitions(Luau.CodeGen PRIVATE ${LUACODEGEN_EXPORT})
target_compile_definitions(minluau PUBLIC ${LUA_IMPORT} ${LUACODE_IMPORT} ${LUACODEGEN_IMPORT})
target_compile_definitions(minluau PRIVATE ${LUA_EXPORT} ${LUACODE_EXPORT} ${LUACODEGEN_EXPORT} MINLUAU_EXPORT_API)
target_include_directories(minluau PRIVATE minluau)
target_include_directories(minluau PUBLIC minluau/include)
target_link_libraries(minluau PUBLIC Luau.VM Luau.Compiler Luau.CodeGen)

target_link_libraries(minluau.cli PRIVATE minluau Luau.Config)
set_target_properties(minluau.cli PROPERTIES OUTPUT_NAME minluau)

target_compile_definitions(minluau.modules PRIVATE MINLUAU_MODULES_EXPORT_API)
target_include_directories(minluau.modules PRIVATE modules)
target_link_libraries(minluau.modules PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    minluau
)

