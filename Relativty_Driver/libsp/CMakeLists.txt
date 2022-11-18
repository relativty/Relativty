# Original file by scott-mudge(github.com/scott-mudge/libserialport-cmake)
#
# Modified for usage in relavtivty

cmake_minimum_required(VERSION 3.2)
project(serialport)

if (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -pedantic")
    set(CMAKE_EXE_LINKER_FLAGS --enable-auto-import)
    add_compile_definitions(LIBSERIALPORT_MSBUILD)
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -pedantic")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS} -s -Os")
set(SOURCE_FILES libserialport/serialport.c libserialport/timing.c)

if (WIN32)
    set (SOURCE_FILES ${SOURCE_FILES} libserialport/windows.c)
    
endif()
if (UNIX)
    set (SOURCE_FILES ${SOURCE_FILES} libserialport/linux.c libserialport/linux_termios.c)
endif()
if(APPLE)
    set (SOURCE_FILES ${SOURCE_FILES} libserialport/macosx.c)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set (SOURCE_FILES ${SOURCE_FILES} libserialport/freebsd.c)
endif()

set(LANGUAGE C)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}) 
include_directories(./libserialport)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} cfgmgr32 SetupAPI)
endif()

