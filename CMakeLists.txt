cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lumin LANGUAGES CXX C)
find_package(Boost REQUIRED COMPONENTS container)

set(luau.dir luau)
set(luau-vm.export "LUA_API=extern \"C\" __declspec(dllexport)")
set(luau-compiler.export "LUACODE_API=extern \"C\" __declspec(dllexport)")
set(luau-codegen.export "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
set(luau-vm.import "LUA_API=extern \"C\" __declspec(dllimport)")
set(luau-compiler.import "LUACODE_API=extern \"C\" __declspec(dllimport)")
set(luau-codegen.import "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")
set(lumin-core.export "LUMIN_CORE_EXPORT_API")
set(lumin-extra.export "LUMIN_EXTRAS_EXPORT_API")

set(lumin-core.dir core)
set(lumin-sdk.dir SDK)
set(lumin-extra.dir extras)
set(lumin.common.dir common)
file(GLOB_RECURSE lumin-core.src ${lumin-core.dir}/*.cpp)
file(GLOB lumin-sdk.src ${lumin-sdk.dir}/*.cpp)
file(GLOB lumin-extra.time.src ${lumin-extra.dir}/time/*.cpp)
file(GLOB lumin-extra.filesystem.src ${lumin-extra.dir}/filesystem/*.cpp)
file(GLOB lumin-extra.console.src ${lumin-extra.dir}/console/*.cpp)

add_subdirectory(${luau.dir})
add_library(lumin-core SHARED ${lumin-core.src})
add_library(lumin.common INTERFACE)
target_include_directories(lumin.common INTERFACE ${lumin.common.dir})
target_link_libraries(lumin.common INTERFACE Boost::container)
if(WIN32)
    add_executable(lumin-sdk WIN32 ${lumin-sdk.src})
else()
    add_executable(lumin-sdk ${lumin-sdk.src})
endif()

add_library(lumin-extra.time SHARED ${lumin-extra.time.src})
add_library(lumin-extra.filesystem SHARED ${lumin-extra.filesystem.src})
add_library(lumin-extra.console SHARED ${lumin-extra.console.src})

add_library(lumin-core.stub SHARED core-stub.cpp)
target_compile_definitions(lumin-core.stub PUBLIC ${luau-vm.import} ${luau-compiler.import} ${luau-codegen.import})
target_compile_definitions(lumin-core.stub PRIVATE ${luau-vm.export} ${luau-compiler.export} ${luau-codegen.export} ${lumin-core.export})
target_include_directories(lumin-core.stub PUBLIC
    ${luau.dir}/Compiler/include
    ${luau.dir}/CodeGen/include
    ${luau.dir}/VM/include
    ${lumin-core.dir}/include)
target_link_libraries(lumin-core.stub PUBLIC lumin.common)

#target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
target_compile_definitions(Luau.VM PRIVATE ${luau-vm.export})
target_compile_definitions(Luau.Compiler PRIVATE ${luau-compiler.export})
target_compile_definitions(Luau.CodeGen PRIVATE ${luau-codegen.export})
target_compile_definitions(lumin-core PUBLIC ${luau-vm.import} ${luau-compiler.import} ${luau-codegen.import})
target_compile_definitions(lumin-core PRIVATE ${luau-vm.export} ${luau-compiler.export} ${luau-codegen.export} ${lumin-core.export})
target_include_directories(lumin-core PRIVATE ${lumin-core.dir})
target_include_directories(lumin-core PUBLIC ${lumin-core.dir}/include)
target_link_libraries(lumin-core PUBLIC Luau.VM Luau.Compiler Luau.CodeGen lumin.common Boost::container)
target_link_libraries(lumin-core PRIVATE Luau.Config)
set(lumin-core.name lumin-core)
set_target_properties(lumin-core PROPERTIES OUTPUT_NAME ${lumin-core.name})

set_target_properties(lumin-core.stub PROPERTIES OUTPUT_NAME ${lumin-core.name})
add_custom_command(TARGET lumin-core.stub POST_BUILD COMMAND ${CMAKE_COMMAND} -E rm -f $<TARGET_FILE:lumin-core.stub>)


target_link_libraries(lumin-sdk PRIVATE lumin-core lumin.common)
set_target_properties(lumin-sdk PROPERTIES OUTPUT_NAME lumin)

target_compile_definitions(lumin-extra.filesystem PRIVATE ${lumin-extra.export})
target_include_directories(lumin-extra.filesystem PRIVATE ${lumin-extra.dir}/include)
set_target_properties(lumin-extra.filesystem PROPERTIES OUTPUT_NAME filesystem.lumin)
target_link_libraries(lumin-extra.filesystem PRIVATE lumin-core.stub lumin.common)

target_compile_definitions(lumin-extra.time PRIVATE ${lumin-extra.export})
target_include_directories(lumin-extra.time PRIVATE ${lumin-extra.dir}/include)
set_target_properties(lumin-extra.time PROPERTIES OUTPUT_NAME time.lumin)
target_link_libraries(lumin-extra.time PRIVATE lumin-core.stub lumin.common)

target_compile_definitions(lumin-extra.console PRIVATE ${lumin-extra.export})
target_include_directories(lumin-extra.console PRIVATE ${lumin-extra.dir}/include)
set_target_properties(lumin-extra.console PROPERTIES OUTPUT_NAME console.lumin)
target_link_libraries(lumin-extra.console PRIVATE lumin-core.stub lumin.common)

