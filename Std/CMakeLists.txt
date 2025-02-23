set(Dluau.Std.EXPORT "DLUAUSTD_EXPORT")
set(Dluau.Std.SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Std common
file(GLOB Dluau.Std.SOURCES ${Dluau.Std.SOURCE_DIR}/*.cpp)
add_library(Dluau.Std STATIC ${Dluau.Std.SOURCES})
target_include_directories(Dluau.Std PUBLIC ${Dluau.Std.SOURCE_DIR})
target_compile_definitions(Dluau.Std PUBLIC ${Dluau.Std.EXPORT})
target_link_libraries(Dluau.Std PUBLIC Dluau.Core Boost::container)

# Std.Time
file(GLOB Dluau.Std.Time.SOURCES ${Dluau.Std.SOURCE_DIR}/Time/*.cpp)
add_library(Dluau.Std.Time SHARED ${Dluau.Std.Time.SOURCES})
target_link_libraries(Dluau.Std.Time PRIVATE Dluau.Std)
set_target_properties(Dluau.Std.Time PROPERTIES OUTPUT_NAME dluau-std_time)

# Std.Fs
file(GLOB Dluau.Std.Fs.SOURCES ${Dluau.Std.SOURCE_DIR}/Fs/*.cpp)
add_library(Dluau.Std.Fs SHARED ${Dluau.Std.Fs.SOURCES})
target_link_libraries(Dluau.Std.Fs PRIVATE Dluau.Std)
set_target_properties(Dluau.Std.Fs PROPERTIES OUTPUT_NAME dluau-std_fs)

# Std.Process
file(GLOB Dluau.Std.Process.SOURCES ${Dluau.Std.SOURCE_DIR}/Process/*.cpp)
add_library(Dluau.Std.Process SHARED ${Dluau.Std.Process.SOURCES})
target_compile_definitions(Dluau.Std.Process PRIVATE _WIN32_WINNT=0x0601)
target_link_libraries(Dluau.Std.Process PRIVATE Dluau.Std Boost::process Boost::filesystem)
set_target_properties(Dluau.Std.Process PROPERTIES OUTPUT_NAME dluau-std_process)

# Std.Cinterop
file(GLOB Dluau.Std.Cinterop.SOURCES ${Dluau.Std.SOURCE_DIR}/Cinterop/*.cpp)
add_library(Dluau.Std.Cinterop SHARED ${Dluau.Std.Cinterop.SOURCES})
target_link_libraries(Dluau.Std.Cinterop PRIVATE Dluau.Std dyncall_s)
target_include_directories(Dluau.Std.Cinterop PRIVATE ${DynCall_DIR})
set_target_properties(Dluau.Std.Cinterop PROPERTIES OUTPUT_NAME dluau-std_cinterop)

# Std.OS
file(GLOB Dluau.Std.OS.SOURCES ${Dluau.Std.SOURCE_DIR}/OS/*.cpp)
add_library(Dluau.Std.OS SHARED ${Dluau.Std.OS.SOURCES})
target_link_libraries(Dluau.Std.OS PRIVATE
    Dluau.Std
)
set_target_properties(Dluau.Std.OS PROPERTIES OUTPUT_NAME dluau-std_os)

# Std.IO
file(GLOB Dluau.Std.IO.SOURCES ${Dluau.Std.SOURCE_DIR}/IO/*.cpp)
add_library(Dluau.Std.IO SHARED ${Dluau.Std.IO.SOURCES})
target_link_libraries(Dluau.Std.IO PRIVATE Dluau.Std)
set_target_properties(Dluau.Std.IO PROPERTIES OUTPUT_NAME dluau-std_io)

# Std.Json
file(GLOB Dluau.Std.Json.SOURCES ${Dluau.Std.SOURCE_DIR}/Json/*.cpp)
add_library(Dluau.Std.Json SHARED ${Dluau.Std.Json.SOURCES})
target_link_libraries(Dluau.Std.Json PRIVATE
    Dluau.Std
    nlohmann_json::nlohmann_json
)
set_target_properties(Dluau.Std.Json PROPERTIES OUTPUT_NAME dluau-std_json)

