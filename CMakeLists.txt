cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(dluau LANGUAGES CXX C)
find_package(Boost REQUIRED COMPONENTS container)
add_subdirectory(extern/dyncall-1.4)
set(DynLoad_DIR ${DynCall_SOURCE_DIR}/dynload)
set(DynCall_DIR ${DynCall_SOURCE_DIR}/dyncall)
set(DynCallback_DIR ${DynCall_SOURCE_DIR}/dyncallback)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/nlohmann_json)

set(luau_DIR luau)
set(luau_vm_EXPORT "LUA_API=extern \"C\" __declspec(dllexport)")
set(luau_compiler_EXPORT "LUACODE_API=extern \"C\" __declspec(dllexport)")
set(luau_codegen_EXPORT "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
set(luau_vm_IMPORT "LUA_API=extern \"C\" __declspec(dllimport)")
set(luau_compiler_IMPORT "LUACODE_API=extern \"C\" __declspec(dllimport)")
set(luau_codegen_IMPORT "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")
set(lib_EXPORT "DLUAU_LIB_EXPORT")

set(version_MAJOR 0)
set(version_MINOR 1)
set(version_PATCH 1)

set(lib_DIR src)
set(cli_DIR cli)
set(common_DIR common)
file(GLOB_RECURSE lib_SRC ${lib_DIR}/*.cpp)
file(GLOB cli_SRC ${cli_DIR}/*.cpp)

add_subdirectory(${luau_DIR})
add_library(lib SHARED ${lib_SRC})
add_library(common INTERFACE)
target_include_directories(common INTERFACE ${common_DIR})
target_link_libraries(common INTERFACE Boost::container)
add_executable(cli ${cli_SRC})
if(WIN32)
    file(GLOB windows_host_SRC ${cli_DIR}/windows_host/*.cpp)
    add_executable(windows_host WIN32 ${windows_host_SRC})
    target_link_libraries(windows_host PRIVATE lib)
    set_target_properties(windows_host PROPERTIES OUTPUT_NAME dluau-windows-host)
endif()


target_compile_definitions(Luau.VM PRIVATE ${luau_vm_EXPORT})
target_compile_definitions(Luau.Compiler PRIVATE ${luau_compiler_EXPORT})
target_compile_definitions(Luau.CodeGen PRIVATE ${luau_codegen_EXPORT})
target_compile_definitions(lib PUBLIC
    ${luau_vm_IMPORT}
    ${luau_compiler_IMPORT}
    ${luau_codegen_IMPORT}
)
target_compile_definitions(lib PRIVATE
    ${luau_vm_EXPORT}
    ${luau_compiler_EXPORT}
    ${luau_codegen_EXPORT}
    ${lib_EXPORT}
    DLUAU_VERSION_MAJOR=${version_MAJOR}
    DLUAU_VERSION_MINOR=${version_MINOR}
    DLUAU_VERSION_PATCH=${version_PATCH}
)
target_include_directories(lib PRIVATE ${lib_DIR} ${DynCall_DIR} ${DynCallback_DIR})
target_include_directories(lib PUBLIC ${lib_DIR}/include)
target_link_libraries(lib PUBLIC
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    common
    Boost::container
    dyncall_s
    dyncallback_s
    nlohmann_json::nlohmann_json
)
set(lib_NAME dluaulib)
set_target_properties(lib PROPERTIES OUTPUT_NAME ${lib_NAME})

target_link_libraries(cli PRIVATE
    lib
)
set_target_properties(cli PROPERTIES OUTPUT_NAME dluau)


