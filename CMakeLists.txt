cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(halua LANGUAGES CXX C)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(luau-dll)
set_target_properties(Luau.VM PROPERTIES OUTPUT_NAME luau-vm)
set_target_properties(Luau.Compiler PROPERTIES OUTPUT_NAME luau-compiler)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)
file (GLOB_RECURSE COMMON_SOURCE ${COMMON_DIR}/*.cpp)
add_library (halua-common STATIC ${COMMON_SOURCE})
target_include_directories(halua-common PUBLIC ${COMMON_DIR})
target_link_libraries(halua-common PRIVATE Luau.VM)

file (GLOB_RECURSE LIB_SOURCE ${CMAKE_SOURCE_DIR}/library/*.cpp)
add_library(halua-lib SHARED ${LIB_SOURCE})
target_include_directories(halua-lib PRIVATE ${CMAKE_SOURCE_DIR}/library)
target_include_directories(halua-lib PUBLIC ${CMAKE_SOURCE_DIR}/library/include)
target_compile_definitions(halua-lib PRIVATE HALUA_API_EXPORT)
target_link_libraries(halua-lib PRIVATE Luau.VM halua-common)
file (GLOB_RECURSE SDL2_SOURCE ${CMAKE_SOURCE_DIR}/sdl2/*.cpp)
add_library(halua-sdl2 SHARED ${SDL2_SOURCE})
target_compile_definitions(halua-sdl2 PRIVATE HALUASDL2_API_EXPORT)
target_link_libraries(halua-sdl2 PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    Luau.VM
    halua-lib
    halua-common
)

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)
if (LINUX)
    add_executable(halua ${SOURCE_FILES})
elseif(WIN32)
    add_executable(halua WIN32 ${SOURCE_FILES})
endif()
target_link_libraries(halua PRIVATE
    Luau.VM
    Luau.Compiler
    Luau.Config
    halua-lib
    halua-common
    halua-sdl2
)
