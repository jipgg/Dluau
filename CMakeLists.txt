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

set(LUMINLIB_DIR lib)
set(LUMINSDK_DIR sdk)
set(LUMINSTD_DIR std)
file(GLOB_RECURSE LUMINLIB_SOURCE ${LUMINLIB_DIR}/*.cpp)
file(GLOB_RECURSE LUMINSDK_SOURCE ${LUMINSDK_DIR}/*.cpp)
file(GLOB_RECURSE LUMINSTD_SOURCE ${LUMINSTD_DIR}/*.cpp)

add_subdirectory(luau)
add_library(lumin-common INTERFACE)
add_library(lumin-lib SHARED ${LUMINLIB_SOURCE})
add_executable(lumin-sdk WIN32 ${LUMINSDK_SOURCE})
add_library(lumin-std SHARED ${LUMINSTD_SOURCE})

target_include_directories(lumin-common INTERFACE common)

target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
target_compile_definitions(Luau.VM PRIVATE ${LUA_EXPORT})
target_compile_definitions(Luau.Compiler PRIVATE ${LUACODE_EXPORT})
target_compile_definitions(Luau.CodeGen PRIVATE ${LUACODEGEN_EXPORT})
target_compile_definitions(lumin-lib PUBLIC ${LUA_IMPORT} ${LUACODE_IMPORT} ${LUACODEGEN_IMPORT})
target_compile_definitions(lumin-lib PRIVATE ${LUA_EXPORT} ${LUACODE_EXPORT} ${LUACODEGEN_EXPORT} LUMIN_EXPORT_API)
target_include_directories(lumin-lib PRIVATE ${LUMINLIB_DIR})
target_include_directories(lumin-lib PUBLIC ${LUMINLIB_DIR}/public)
target_link_libraries(lumin-lib PUBLIC
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    lumin-common
)
target_link_libraries(lumin-lib PRIVATE Luau.Config)
set_target_properties(lumin-lib PROPERTIES OUTPUT_NAME luminlib)

target_link_libraries(lumin-sdk PRIVATE lumin-lib Luau.Config)
set_target_properties(lumin-sdk PROPERTIES OUTPUT_NAME lumin)

target_compile_definitions(lumin-std PRIVATE LUMINSTD_EXPORT_API)
set_target_properties(lumin-std PROPERTIES OUTPUT_NAME luminstd)
target_link_options(lumin-std PRIVATE /NODEFAULTLIB:LIBCMTD)
target_include_directories(lumin-std PRIVATE std)
target_link_libraries(lumin-std PRIVATE
    SDL2::SDL2-static
    SDL2_ttf::SDL2_ttf-static
    SDL2_image::SDL2_image-static
    SDL2_mixer::SDL2_mixer-static
    lumin-lib
)

