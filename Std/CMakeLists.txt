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
set_target_properties(Dluau.Std.Time PROPERTIES OUTPUT_NAME dluau_std_time)

# Std.Fs
file(GLOB Dluau.Std.Fs.SOURCES ${Dluau.Std.SOURCE_DIR}/Fs/*.cpp)
add_library(Dluau.Std.Fs SHARED ${Dluau.Std.Fs.SOURCES})
target_link_libraries(Dluau.Std.Fs PRIVATE Dluau.Std)
set_target_properties(Dluau.Std.Fs PROPERTIES OUTPUT_NAME dluau_std_fs)

# Std.Process
file(GLOB Dluau.Std.Process.SOURCES ${Dluau.Std.SOURCE_DIR}/Process/*.cpp)
add_library(Dluau.Std.Process SHARED ${Dluau.Std.Process.SOURCES})
target_compile_definitions(Dluau.Std.Process PRIVATE _WIN32_WINNT=0x0601)
target_link_libraries(Dluau.Std.Process PRIVATE Dluau.Std Boost::process Boost::filesystem)
set_target_properties(Dluau.Std.Process PROPERTIES OUTPUT_NAME dluau_std_process)

file(GLOB Dluau.Std.Cinterop.SOURCES ${Dluau.Std.SOURCE_DIR}/Cinterop/*.cpp)
add_library(Dluau.Std.Cinterop SHARED ${Dluau.Std.Cinterop.SOURCES})
target_link_libraries(Dluau.Std.Cinterop PRIVATE Dluau.Std dyncall_s)
target_include_directories(Dluau.Std.Cinterop PRIVATE ${DynCall_DIR})
set_target_properties(Dluau.Std.Cinterop PROPERTIES OUTPUT_NAME dluau_std_cinterop)

file(GLOB Dluau.Std.Os.SOURCES ${Dluau.Std.SOURCE_DIR}/Os/*.cpp)
add_library(Dluau.Std.Os SHARED ${Dluau.Std.Os.SOURCES})
target_link_libraries(Dluau.Std.Os PRIVATE Dluau.Std)
set_target_properties(Dluau.Std.Os PROPERTIES OUTPUT_NAME dluau_std_os)

file(GLOB Dluau.Std.Io.SOURCES ${Dluau.Std.SOURCE_DIR}/Io/*.cpp)
add_library(Dluau.Std.Io SHARED ${Dluau.Std.Io.SOURCES})
target_link_libraries(Dluau.Std.Io PRIVATE Dluau.Std)
set_target_properties(Dluau.Std.Io PROPERTIES OUTPUT_NAME dluau_std_io)
