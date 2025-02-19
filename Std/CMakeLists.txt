set(Dluau.Std.EXPORT "DLUAUSTD_EXPORT")

set(Dluau.Std.SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB Dluau.Std.SOURCES ${Dluau.Std.SOURCE_DIR}/*.cpp)
file(GLOB Dluau.Std.Time.SOURCES ${Dluau.Std.SOURCE_DIR}/Time/*.cpp)
file(GLOB Dluau.Std.Fs.SOURCES ${Dluau.Std.SOURCE_DIR}/Fs/*.cpp)
file(GLOB Dluau.Std.Process.SOURCES ${Dluau.Std.SOURCE_DIR}/Process/*.cpp)
file(GLOB Dluau.Std.Cinterop.SOURCES ${Dluau.Std.SOURCE_DIR}/Cinterop/*.cpp)

add_library(Dluau.Std STATIC ${Dluau.Std.SOURCES})
add_library(Dluau.Std.Time SHARED ${Dluau.Std.Time.SOURCES})
add_library(Dluau.Std.Fs SHARED ${Dluau.Std.Fs.SOURCES})
add_library(Dluau.Std.Process SHARED ${Dluau.Std.Process.SOURCES})
add_library(Dluau.Std.Cinterop SHARED ${Dluau.Std.Cinterop.SOURCES})

target_include_directories(Dluau.Std PUBLIC ${Dluau.Std.SOURCE_DIR})
target_compile_definitions(Dluau.Std PUBLIC ${Dluau.Std.EXPORT})
target_link_libraries(Dluau.Std PUBLIC Dluau.Core Boost::container)
target_link_libraries(Dluau.Std.Time PRIVATE Dluau.Std)
target_link_libraries(Dluau.Std.Fs PRIVATE Dluau.Std)
target_link_libraries(Dluau.Std.Cinterop PRIVATE Dluau.Std dyncall_s)
target_include_directories(Dluau.Std.Cinterop PRIVATE ${DynCall_DIR})

target_compile_definitions(Dluau.Std.Process PRIVATE
    _WIN32_WINNT=0x0601
    VCPKG_TARGET_TRIPLET=x64-windows-static
)
target_link_libraries(Dluau.Std.Process PRIVATE
    Dluau.Std
    Boost::process
    Boost::filesystem
)
set_target_properties(Dluau.Std.Time PROPERTIES OUTPUT_NAME dluau_std_time)
set_target_properties(Dluau.Std.Fs PROPERTIES OUTPUT_NAME dluau_std_fs)
set_target_properties(Dluau.Std.Process PROPERTIES OUTPUT_NAME dluau_std_process)
set_target_properties(Dluau.Std.Cinterop PROPERTIES OUTPUT_NAME dluau_std_cinterop)
