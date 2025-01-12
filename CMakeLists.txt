cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Lumin LANGUAGES CXX C)

set(Luau.Directory luau)
set(Luau.VM.Export "LUA_API=extern \"C\" __declspec(dllexport)")
set(Luau.Compiler.Export "LUACODE_API=extern \"C\" __declspec(dllexport)")
set(Luau.CodeGen.Export "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
set(Luau.VM.Import "LUA_API=extern \"C\" __declspec(dllimport)")
set(Luau.Compiler.Import "LUACODE_API=extern \"C\" __declspec(dllimport)")
set(Luau.CodeGen.Import "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")
set(Lumin.Core.Export "LUMIN_CORE_EXPORT_API")
set(Lumin.Extras.Export "LUMIN_EXTRAS_EXPORT_API")

set(Lumin.Core.Directory Core)
set(Lumin.SDK.Directory SDK)
set(Lumin.Extras.Directory Extras)
set(Lumin.Utility.Directory Utility)
file(GLOB Lumin.Core.Sources ${Lumin.Core.Directory}/*.cpp)
file(GLOB Lumin.SDK.Sources ${Lumin.SDK.Directory}/*.cpp)
file(GLOB Lumin.Extras.Date_Time.Sources ${Lumin.Extras.Directory}/Date_Time/*.cpp)
file(GLOB Lumin.Extras.Filesystem.Sources ${Lumin.Extras.Directory}/Filesystem/*.cpp)
file(GLOB Lumin.Extras.Console.Sources ${Lumin.Extras.Directory}/Console/*.cpp)

add_subdirectory(${Luau.Directory})
add_library(Lumin.Utility INTERFACE)
add_library(Lumin.Core SHARED ${Lumin.Core.Sources})
add_executable(Lumin.SDK WIN32 ${Lumin.SDK.Sources})

add_library(Lumin.Extras.Date_Time SHARED ${Lumin.Extras.Date_Time.Sources})
add_library(Lumin.Extras.Filesystem SHARED ${Lumin.Extras.Filesystem.Sources})
add_library(Lumin.Extras.Console SHARED ${Lumin.Extras.Console.Sources})

target_include_directories(Lumin.Utility INTERFACE ${Lumin.Utility.Directory})
target_link_libraries(Lumin.Utility INTERFACE Lumin.Core)

target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
target_compile_definitions(Luau.VM PRIVATE ${Luau.VM.Export})
target_compile_definitions(Luau.Compiler PRIVATE ${Luau.Compiler.Export})
target_compile_definitions(Luau.CodeGen PRIVATE ${Luau.CodeGen.Export})
target_compile_definitions(Lumin.Core PUBLIC ${Luau.VM.Import} ${Luau.Compiler.Import} ${Luau.CodeGen.Import})
target_compile_definitions(Lumin.Core PRIVATE ${Luau.VM.Export} ${Luau.Compiler.Export} ${Luau.CodeGen.Export} ${Lumin.Core.Export})
target_include_directories(Lumin.Core PRIVATE ${Lumin.Core.Directory})
target_include_directories(Lumin.Core PUBLIC ${Lumin.Core.Directory}/include)
target_link_libraries(Lumin.Core PUBLIC Luau.VM Luau.Compiler Luau.CodeGen Lumin.Utility)
target_link_libraries(Lumin.Core PRIVATE Luau.Config)
set(Lumin.Core.Output_Name lumin-core)
set_target_properties(Lumin.Core PROPERTIES OUTPUT_NAME ${Lumin.Core.Output_Name})

add_library(Lumin.Core.Stub SHARED Core_API_Stub_Definitions.cpp)
target_compile_definitions(Lumin.Core.Stub PUBLIC ${Luau.VM.Import} ${Luau.Compiler.Import} ${Luau.CodeGen.Import})
target_compile_definitions(Lumin.Core.Stub PRIVATE ${Luau.VM.Export} ${Luau.Compiler.Export} ${Luau.CodeGen.Export} ${Lumin.Core.Export})
target_include_directories(Lumin.Core.Stub PUBLIC
    ${Luau.Directory}/Compiler/include
    ${Luau.Directory}/CodeGen/include
    ${Luau.Directory}/VM/include
    ${Lumin.Core.Directory}/include
    ${Lumin.Utility.Directory}
)
set_target_properties(Lumin.Core.Stub PROPERTIES OUTPUT_NAME ${Lumin.Core.Output_Name})
add_custom_command(
    TARGET Lumin.Core.Stub POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f $<TARGET_FILE:Lumin.Core.Stub>
    COMMENT "Removing the ${Lumin.Core.Output_Name} stub library after link."
)

target_link_libraries(Lumin.SDK PRIVATE Lumin.Core.Stub)
target_compile_definitions(Lumin.Extras.Filesystem PUBLIC ${Luau.VM.Import} ${Luau.Compiler.Import} ${Luau.CodeGen.Import})
set_target_properties(Lumin.SDK PROPERTIES OUTPUT_NAME lumin)

# standard module libraries
target_compile_definitions(Lumin.Extras.Filesystem PRIVATE ${Lumin.Extras.Export})
target_compile_definitions(Lumin.Extras.Filesystem PUBLIC ${Luau.VM.Import} ${Luau.Compiler.Import} ${Luau.CodeGen.Import})
target_include_directories(Lumin.Extras.Filesystem PRIVATE ${Lumin.Extras.Directory}/include)
set_target_properties(Lumin.Extras.Filesystem PROPERTIES OUTPUT_NAME lumin-extras-filesystem)
target_link_libraries(Lumin.Extras.Filesystem PRIVATE Lumin.Core.Stub)

target_compile_definitions(Lumin.Extras.Date_Time PRIVATE ${Lumin.Extras.Export})
target_include_directories(Lumin.Extras.Date_Time PRIVATE ${Lumin.Extras.Directory}/include)
set_target_properties(Lumin.Extras.Date_Time PROPERTIES OUTPUT_NAME lumin-extras-date_time)
target_link_libraries(Lumin.Extras.Date_Time PRIVATE Lumin.Core.Stub)

target_compile_definitions(Lumin.Extras.Console PRIVATE ${Lumin.Extras.Export})
target_include_directories(Lumin.Extras.Console PRIVATE ${Lumin.Extras.Directory}/include)
set_target_properties(Lumin.Extras.Console PROPERTIES OUTPUT_NAME lumin-extras-console)
target_link_libraries(Lumin.Extras.Console PRIVATE Lumin.Core.Stub)
