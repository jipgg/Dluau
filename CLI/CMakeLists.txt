# CLI settings
set(Dluau.CLI.SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB Dluau.CLI.SOURCES ${Dluau.CLI.SOURCE_DIR}/*.cpp)

add_executable(Dluau.CLI ${Dluau.CLI.SOURCES})
target_link_libraries(Dluau.CLI PRIVATE Dluau.Core)
set_target_properties(Dluau.CLI PROPERTIES OUTPUT_NAME dluau)

# Windows Host settings
if(WIN32)
    file(GLOB Dluau.CLI.Winhost.SOURCES ${Dluau.CLI.SOURCE_DIR}/windows_host/*.cpp)
    add_executable(Dluau.CLI.Winhost WIN32 ${Dluau.CLI.Winhost.SOURCES})
    target_link_libraries(Dluau.CLI.Winhost PRIVATE Dluau.Core)
    set_target_properties(Dluau.CLI.Winhost PROPERTIES OUTPUT_NAME dluau-windows-host)
endif()
