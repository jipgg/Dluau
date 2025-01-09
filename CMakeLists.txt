cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(minlu LANGUAGES CXX C)

set(LUA_EXPORT "LUA_API=extern \"C\" __declspec(dllexport)")
set(LUACODE_EXPORT "LUACODE_API=extern \"C\" __declspec(dllexport)")
set(LUACODEGEN_EXPORT "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
set(LUA_IMPORT "LUA_API=extern \"C\" __declspec(dllimport)")
set(LUACODE_IMPORT "LUACODE_API=extern \"C\" __declspec(dllimport)")
set(LUACODEGEN_IMPORT "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

file(GLOB_RECURSE MINLU_SOURCE minlu/*.cpp)
file(GLOB_RECURSE CLI_SOURCE cli/*.cpp)
file(GLOB_RECURSE STD_SOURCE standard/*.cpp)

add_subdirectory(luau)
add_library(minlu SHARED ${MINLU_SOURCE})
add_executable(minlu.cli WIN32 ${CLI_SOURCE})
add_library(minlu.standard SHARED ${STD_SOURCE})

target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
target_compile_definitions(Luau.VM PRIVATE ${LUA_EXPORT})
target_compile_definitions(Luau.Compiler PRIVATE ${LUACODE_EXPORT})
target_compile_definitions(Luau.CodeGen PRIVATE ${LUACODEGEN_EXPORT})
target_compile_definitions(minlu PUBLIC ${LUA_IMPORT} ${LUACODE_IMPORT} ${LUACODEGEN_IMPORT})
target_compile_definitions(minlu PRIVATE ${LUA_EXPORT} ${LUACODE_EXPORT} ${LUACODEGEN_EXPORT} MINLU_EXPORT_API)
target_include_directories(minlu PRIVATE minlu)
target_include_directories(minlu PUBLIC minlu/include)
target_link_libraries(minlu PUBLIC Luau.VM Luau.Compiler Luau.CodeGen)

target_link_libraries(minlu.cli PRIVATE minlu Luau.Config)
set_target_properties(minlu.cli PROPERTIES OUTPUT_NAME minlu)

target_compile_definitions(minlu.standard PRIVATE STDMINLU_EXPORT_API)
set_target_properties(minlu.standard PROPERTIES OUTPUT_NAME stdminlu)
target_link_options(minlu.standard PRIVATE /NODEFAULTLIB:LIBCMTD)
target_include_directories(minlu.standard PRIVATE standard)
target_link_libraries(minlu.standard PRIVATE
    SDL2::SDL2-static
    SDL2_ttf::SDL2_ttf-static
    SDL2_image::SDL2_image-static
    SDL2_mixer::SDL2_mixer-static
    minlu
)

