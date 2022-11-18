cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(WIN32)
	set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
	set(CMAKE_CXX_FLAGS_DEBUG "/DEBUG")
else()
	set(CMAKE_CXX_FLAGS "-Wno-undef")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()


# generate the compile_commands for vscode / clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# set the project name
project(driver_relativty)


set(SOURCES
	source/DriverFactory.cpp
	source/driverlog.cpp
	source/Relativty_EmbeddedPython.cpp
	source/Relativty_HMDDriver.cpp
	source/Relativty_ServerDriver.cpp
)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Relativty/bin/)

#$<0:> remove the Debug & release folder so the binary is directly in the right folder
# CMAKE_ mean that it is applied to all sub-projects so all libraries should automatically be place in the right folder
if(WIN32)
	if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" )
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/win64/$<0:>)
	else()
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/win32/$<0:>)
	endif()
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/linux64/$<0:>)
endif()


# add the executable
add_library(driver_relativty SHARED ${SOURCES})
add_subdirectory(hidapi)
add_subdirectory(libsp)
target_link_libraries(driver_relativty serialport)
target_link_libraries(driver_relativty hidapi::hidapi hidapi::include)

if(WIN32)
	if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" )
		target_link_libraries(driver_relativty	"${CMAKE_SOURCE_DIR}/libraries/x64/python38.lib")
	else()
		target_link_libraries(driver_relativty	"${CMAKE_SOURCE_DIR}/libraries/Win32/python38.lib")
	endif()
	target_include_directories(driver_relativty PRIVATE "libsp/libserialport")

else(WIN32)
	set(SOURCES
		source/stricmp.cpp
		${SOURCES}
	)

	find_package (Python REQUIRED
	        COMPONENTS Interpreter Development)
	target_link_libraries(driver_relativty Python::Python)

	find_package(PkgConfig REQUIRED)
	pkg_search_module(SP REQUIRED libserialport)
	target_include_directories(driver_relativty PRIVATE ${SP_INCLUDE_DIRS})
	target_link_libraries(driver_relativty ${SP_LIBRARIES})
endif(WIN32)

set_property(TARGET driver_relativty PROPERTY C_STANDARD 17)
set_property(TARGET driver_relativty PROPERTY CXX_STANDARD 17)