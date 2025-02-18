# Library settings
set(Dluau.Core.VERSION_MAJOR 0)
set(Dluau.Core.VERSION_MINOR 1)
set(Dluau.Core.VERSION_PATCH 5)

# Luau API export definitions
set(Luau.VM.IMPORT "LUA_API=extern \"C\" __declspec(dllimport)")
set(Luau.Compiler.IMPORT "LUACODE_API=extern \"C\" __declspec(dllimport)")
set(Luau.CodeGen.IMPORT "LUACODEGEN_API=extern \"C\" __declspec(dllimport)")
set(Luau.VM.EXPORT "LUA_API=extern \"C\" __declspec(dllexport)")
set(Luau.Compiler.EXPORT "LUACODE_API=extern \"C\" __declspec(dllexport)")
set(Luau.CodeGen.EXPORT "LUACODEGEN_API=extern \"C\" __declspec(dllexport)")
target_compile_definitions(Luau.VM PRIVATE ${Luau.VM.EXPORT})
target_compile_definitions(Luau.Compiler PRIVATE ${Luau.Compiler.EXPORT})
target_compile_definitions(Luau.CodeGen PRIVATE ${Luau.CodeGen.EXPORT})

set(Dluau.Core.SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE Dluau.Core.SOURCES ${Dluau.Core.SOURCE_DIR}/*.cpp)

add_library(Dluau.Core SHARED ${Dluau.Core.SOURCES})

set(Dluau.Core.EXPORT "DLUAUCORE_EXPORT")

target_compile_definitions(Dluau.Core PUBLIC
    ${Luau.VM.IMPORT}
    ${Luau.Compiler.IMPORT}
    ${Luau.CodeGen.IMPORT}
)
target_compile_definitions(Dluau.Core PRIVATE
    ${Luau.VM.EXPORT}
    ${Luau.Compiler.EXPORT}
    ${Luau.CodeGen.EXPORT}
    ${Dluau.Core.EXPORT}
    DLUAUCORE_VERSION_MAJOR=${Dluau.Core.VERSION_MAJOR}
    DLUAUCORE_VERSION_MINOR=${Dluau.Core.VERSION_MINOR}
    DLUAUCORE_VERSION_PATCH=${Dluau.Core.VERSION_PATCH}
)

target_include_directories(Dluau.Core PRIVATE ${Dluau.Core.SOURCE_DIR})
target_include_directories(Dluau.Core PUBLIC ${Dluau.Core.SOURCE_DIR}/include)

# Dependencies
target_link_libraries(Dluau.Core PUBLIC
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    Boost::container
    nlohmann_json::nlohmann_json
)
# Set output name
set(Dluau.Core.OUTPUT_NAME dluau-core)
set_target_properties(Dluau.Core PROPERTIES OUTPUT_NAME ${Dluau.Core.OUTPUT_NAME})
