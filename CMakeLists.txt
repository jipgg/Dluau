cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(LuauXT LANGUAGES CXX C)
find_package(Boost REQUIRED COMPONENTS container)
add_subdirectory(extern/dyncall-1.4)
set(DynLoad_DIR ${DynCall_SOURCE_DIR}/dynload)
set(DynCall_DIR ${DynCall_SOURCE_DIR}/dyncall)
set(DynCallback_DIR ${DynCall_SOURCE_DIR}/dyncallback)

set(luau_DIR luau)
set(luau_vm_EXPORT "LUA_API=extern \"C\" __declspec(dllexport)")
set(luau_compiler_EXPORT "LUACODE_API=extern \"C\" __declspec(dllexport)")
set(luau_codegen_EXPORT "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
set(luau_vm_IMPORT "LUA_API=extern \"C\" __declspec(dllimport)")
set(luau_compiler_IMPORT "LUACODE_API=extern \"C\" __declspec(dllimport)")
set(luau_codegen_IMPORT "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")
set(lib_EXPORT "GOLUAU_API_EXPORT")
set(modules_EXPORT "GOLUAU_MODULES_API_EXPORT")

set(lib_DIR lib)
set(cli_DIR cli)
set(modules_DIR modules)
set(common_DIR common)
set(host_DIR host)
file(GLOB_RECURSE lib_SRC ${lib_DIR}/*.cpp)
file(GLOB cli_SRC ${cli_DIR}/*.cpp)
file(GLOB time_module_SRC ${modules_DIR}/time/*.cpp)
file(GLOB filesystem_module_SRC ${modules_DIR}/filesystem/*.cpp)
file(GLOB console_module_SRC ${modules_DIR}/console/*.cpp)
file(GLOB host_SRC ${host_DIR}/*.cpp)

add_subdirectory(${luau_DIR})
add_library(lib SHARED ${lib_SRC})
add_library(common INTERFACE)
target_include_directories(common INTERFACE ${common_DIR})
target_link_libraries(common INTERFACE Boost::container)
add_executable(cli ${cli_SRC})
if(WIN32)
    file (GLOB host_WINDOWS ${host_DIR}/windows/*.cpp)
    add_executable(host WIN32 ${host_SRC} ${host_WINDOWS})
else()
    file (GLOB host_LINUX ${host_DIR}/linux/*.cpp)
    add_executable(host ${host_SRC} ${host_LINUX})
endif()

add_library(time_module SHARED ${time_module_SRC})
add_library(filesystem_module SHARED ${filesystem_module_SRC})
add_library(console_module SHARED ${console_module_SRC})

add_library(lib_stub SHARED libstub.cpp)
target_compile_definitions(lib_stub PUBLIC
    ${luau_vm_IMPORT}
    ${luau_compiler_IMPORT}
    ${luau_codegen_IMPORT}
)
target_compile_definitions(lib_stub PRIVATE
    ${luau_vm_EXPORT}
    ${luau_compiler_EXPORT}
    ${luau_codegen_EXPORT}
    ${lib_EXPORT}
)
target_include_directories(lib_stub PUBLIC
    ${luau_DIR}/Compiler/include
    ${luau_DIR}/CodeGen/include
    ${luau_DIR}/VM/include
    ${lib_DIR}/include)
target_link_libraries(lib_stub PUBLIC common)

target_link_libraries(host PRIVATE lib_stub)
set_target_properties(host PROPERTIES OUTPUT_NAME LuauXT_host)

#target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
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
)
target_include_directories(lib PRIVATE ${lib_DIR} ${DynCall_DIR})
target_include_directories(lib PUBLIC ${lib_DIR}/include)
target_link_libraries(lib PUBLIC
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    common
    Boost::container
    dyncall_s
)
target_link_libraries(lib PRIVATE Luau.Config)
set(lib_NAME LuauXT)
set_target_properties(lib PROPERTIES OUTPUT_NAME ${lib_NAME})

set_target_properties(lib_stub PROPERTIES OUTPUT_NAME ${lib_NAME})
add_custom_command(TARGET lib_stub POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E rm -f $<TARGET_FILE:lib_stub>
)
target_link_libraries(cli PRIVATE lib)
set_target_properties(cli PROPERTIES OUTPUT_NAME ${lib_NAME})

target_compile_definitions(filesystem_module PRIVATE ${modules_EXPORT})
target_include_directories(filesystem_module PRIVATE ${modules_DIR}/include)
set_target_properties(filesystem_module PROPERTIES OUTPUT_NAME xtm_filesystem)
target_link_libraries(filesystem_module PRIVATE lib)

target_compile_definitions(time_module PRIVATE ${modules_EXPORT})
target_include_directories(time_module PRIVATE ${modules_DIR}/include)
set_target_properties(time_module PROPERTIES OUTPUT_NAME xtm_time)
target_link_libraries(time_module PRIVATE lib)

target_compile_definitions(console_module PRIVATE ${modules_EXPORT})
target_include_directories(console_module PRIVATE ${modules_DIR}/include)
set_target_properties(console_module PROPERTIES OUTPUT_NAME xtm_console)
target_link_libraries(console_module PRIVATE lib)

