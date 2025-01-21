cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(goluau LANGUAGES CXX C)
find_package(Boost REQUIRED COMPONENTS container)

set(luau_DIR luau)
set(luau_vm_EXPORT "LUA_API=extern \"C\" __declspec(dllexport)")
set(luau_compiler_EXPORT "LUACODE_API=extern \"C\" __declspec(dllexport)")
set(luau_codegen_EXPORT "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
set(luau_vm_IMPORT "LUA_API=extern \"C\" __declspec(dllimport)")
set(luau_compiler_IMPORT "LUACODE_API=extern \"C\" __declspec(dllimport)")
set(luau_codegen_IMPORT "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")
set(goluau_lib_EXPORT "GOLUAU_API_EXPORT")
set(goluau_modules_EXPORT "GOLUAU_MODULES_API_EXPORT")

set(goluau_lib_DIR lib)
set(goluau_cli_DIR cli)
set(goluau_modules_DIR modules)
set(goluau_common_DIR common)
set(goluau_host_DIR host)
file(GLOB_RECURSE goluau_lib_SRC ${goluau_lib_DIR}/*.cpp)
file(GLOB goluau_cli_SRC ${goluau_cli_DIR}/*.cpp)
file(GLOB goluau_time_module_SRC ${goluau_modules_DIR}/time/*.cpp)
file(GLOB goluau_fs_module_SRC ${goluau_modules_DIR}/fs/*.cpp)
file(GLOB goluau_console_module_SRC ${goluau_modules_DIR}/console/*.cpp)
file(GLOB goluau_host_SRC ${goluau_host_DIR}/*.cpp)

add_subdirectory(${luau_DIR})
add_library(goluau_lib SHARED ${goluau_lib_SRC})
add_library(goluau_common INTERFACE)
target_include_directories(goluau_common INTERFACE ${goluau_common_DIR})
target_link_libraries(goluau_common INTERFACE Boost::container)
add_executable(goluau_cli ${goluau_cli_SRC})
if(WIN32)
    file (GLOB goluau_host_WINDOWS ${goluau_host_DIR}/windows/*.cpp)
    add_executable(goluau_host WIN32 ${goluau_host_SRC} ${goluau_host_WINDOWS})
else()
    file (GLOB goluau_host_LINUX ${goluau_host_DIR}/linux/*.cpp)
    add_executable(goluau_host ${goluau_host_SRC} ${goluau_host_LINUX})
endif()

add_library(goluau_time_module SHARED ${goluau_time_module_SRC})
add_library(goluau_fs_module SHARED ${goluau_fs_module_SRC})
add_library(goluau_console_module SHARED ${goluau_console_module_SRC})

add_library(goluau_lib_stub SHARED libstub.cpp)
target_compile_definitions(goluau_lib_stub PUBLIC
    ${luau_vm_IMPORT}
    ${luau_compiler_IMPORT}
    ${luau_codegen_IMPORT}
)
target_compile_definitions(goluau_lib_stub PRIVATE
    ${luau_vm_EXPORT}
    ${luau_compiler_EXPORT}
    ${luau_codegen_EXPORT}
    ${goluau_lib_EXPORT}
)
target_include_directories(goluau_lib_stub PUBLIC
    ${luau_DIR}/Compiler/include
    ${luau_DIR}/CodeGen/include
    ${luau_DIR}/VM/include
    ${goluau_lib_DIR}/include)
target_link_libraries(goluau_lib_stub PUBLIC goluau_common)

target_link_libraries(goluau_host PRIVATE goluau_lib_stub)
set_target_properties(goluau_host PROPERTIES OUTPUT_NAME goluau-host)

#target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
target_compile_definitions(Luau.VM PRIVATE ${luau_vm_EXPORT})
target_compile_definitions(Luau.Compiler PRIVATE ${luau_compiler_EXPORT})
target_compile_definitions(Luau.CodeGen PRIVATE ${luau_codegen_EXPORT})
target_compile_definitions(goluau_lib PUBLIC
    ${luau_vm_IMPORT}
    ${luau_compiler_IMPORT}
    ${luau_codegen_IMPORT}
)
target_compile_definitions(goluau_lib PRIVATE
    ${luau_vm_EXPORT}
    ${luau_compiler_EXPORT}
    ${luau_codegen_EXPORT}
    ${goluau_lib_EXPORT}
)
target_include_directories(goluau_lib PRIVATE ${goluau_lib_DIR})
target_include_directories(goluau_lib PUBLIC ${goluau_lib_DIR}/include)
target_link_libraries(goluau_lib PUBLIC
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    goluau_common
    Boost::container
)
target_link_libraries(goluau_lib PRIVATE Luau.Config)
set(goluau_lib_NAME goluau)
set_target_properties(goluau_lib PROPERTIES OUTPUT_NAME ${goluau_lib_NAME})

set_target_properties(goluau_lib_stub PROPERTIES OUTPUT_NAME ${goluau_lib_NAME})
add_custom_command(TARGET goluau_lib_stub POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E rm -f $<TARGET_FILE:goluau_lib_stub>
)
target_link_libraries(goluau_cli PRIVATE goluau_lib)

target_compile_definitions(goluau_fs_module PRIVATE ${goluau_modules_EXPORT})
target_include_directories(goluau_fs_module PRIVATE ${goluau_modules_DIR}/include)
set_target_properties(goluau_fs_module PROPERTIES OUTPUT_NAME module_fs)
target_link_libraries(goluau_fs_module PRIVATE goluau_lib)

target_compile_definitions(goluau_time_module PRIVATE ${goluau_modules_EXPORT})
target_include_directories(goluau_time_module PRIVATE ${goluau_modules_DIR}/include)
set_target_properties(goluau_time_module PROPERTIES OUTPUT_NAME module_time)
target_link_libraries(goluau_time_module PRIVATE goluau_lib)

target_compile_definitions(goluau_console_module PRIVATE ${goluau_modules_EXPORT})
target_include_directories(goluau_console_module PRIVATE ${goluau_modules_DIR}/include)
set_target_properties(goluau_console_module PROPERTIES OUTPUT_NAME module_console)
target_link_libraries(goluau_console_module PRIVATE goluau_lib)

