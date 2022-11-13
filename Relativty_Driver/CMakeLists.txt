cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-fms-extensions")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address -O0")
set(CMAKE_CXX_FLAGS_RELEASE "")

# generate the compile_commands for vscode / clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# set the project name
project(relativty)


set(SOURCES
	source/stricmp.c
	source/DriverFactory.cpp
	source/driverlog.cpp
	source/Relativty_EmbeddedPython.cpp
	source/Relativty_HMDDriver.cpp
	source/Relativty_ServerDriver.cpp
)

# add the executable
add_library(relativty ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_search_module(HID REQUIRED libhid)
pkg_search_module(PYTHON REQUIRED python3)

target_include_directories(relativty PRIVATE ${HID_INCLUDE_DIRS})
target_link_libraries(relativty ${HID_LIBRARIES})

target_include_directories(relativty PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(relativty ${PYTHON_LIBRARIES})

set_property(TARGET relativty PROPERTY C_STANDARD 17)
set_property(TARGET relativty PROPERTY CXX_STANDARD 17)
