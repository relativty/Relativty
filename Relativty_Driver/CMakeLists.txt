cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wno-undef")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# generate the compile_commands for vscode / clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# set the project name
project(relativty)


set(SOURCES
	source/stricmp.cpp
	source/DriverFactory.cpp
	source/driverlog.cpp
	source/Relativty_EmbeddedPython.cpp
	source/Relativty_HMDDriver.cpp
	source/Relativty_ServerDriver.cpp
)

add_subdirectory(hidapi)

# add the executable
add_library(relativty SHARED ${SOURCES})

find_package (Python REQUIRED
        COMPONENTS Interpreter Development)

target_link_libraries(relativty Python::Python)
target_link_libraries(relativty hidapi::hidapi hidapi::include)

find_package(PkgConfig REQUIRED)
pkg_search_module(SP REQUIRED libserialport)
target_include_directories(relativty PRIVATE ${SP_INCLUDE_DIRS})
target_link_libraries(relativty ${SP_LIBRARIES})

set_property(TARGET relativty PROPERTY C_STANDARD 17)
set_property(TARGET relativty PROPERTY CXX_STANDARD 17)
