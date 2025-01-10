cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lumin LANGUAGES CXX C)

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

file(GLOB_RECURSE LUMIN_META_SOURCE meta/*.cpp)
file(GLOB_RECURSE LUMIN_SDK_SOURCE sdk/*.cpp)
file(GLOB_RECURSE LUMIN_STDLIB_SOURCE stdlib/*.cpp)

add_subdirectory(luau)
add_library(lumin.common INTERFACE)
add_library(lumin.meta SHARED ${LUMIN_META_SOURCE})
add_executable(lumin.sdk WIN32 ${LUMIN_SDK_SOURCE})
add_library(lumin.stdlib SHARED ${LUMIN_STDLIB_SOURCE})

target_include_directories(lumin.common INTERFACE common)

target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
target_compile_definitions(Luau.VM PRIVATE ${LUA_EXPORT})
target_compile_definitions(Luau.Compiler PRIVATE ${LUACODE_EXPORT})
target_compile_definitions(Luau.CodeGen PRIVATE ${LUACODEGEN_EXPORT})
target_compile_definitions(lumin.meta PUBLIC ${LUA_IMPORT} ${LUACODE_IMPORT} ${LUACODEGEN_IMPORT})
target_compile_definitions(lumin.meta PRIVATE ${LUA_EXPORT} ${LUACODE_EXPORT} ${LUACODEGEN_EXPORT} LUMIN_EXPORT_API)
target_include_directories(lumin.meta PRIVATE meta)
target_include_directories(lumin.meta PUBLIC meta/include)
target_link_libraries(lumin.meta PUBLIC
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    lumin.common
)
target_link_libraries(lumin.meta PRIVATE Luau.Config)
set_target_properties(lumin.meta PROPERTIES OUTPUT_NAME lumin-meta)

target_link_libraries(lumin.sdk PRIVATE lumin.meta Luau.Config)
set_target_properties(lumin.sdk PROPERTIES OUTPUT_NAME lumin)

target_compile_definitions(lumin.stdlib PRIVATE LUMINSTD_EXPORT_API)
set_target_properties(lumin.stdlib PROPERTIES OUTPUT_NAME lumin-stdlib)
target_link_options(lumin.stdlib PRIVATE /NODEFAULTLIB:LIBCMTD)
target_include_directories(lumin.stdlib PRIVATE stdlib)
target_link_libraries(lumin.stdlib PRIVATE
    SDL2::SDL2-static
    SDL2_ttf::SDL2_ttf-static
    SDL2_image::SDL2_image-static
    SDL2_mixer::SDL2_mixer-static
    lumin.meta
)

