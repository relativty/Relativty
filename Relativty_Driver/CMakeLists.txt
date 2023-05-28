cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(MINGW)
	message("Do not use mingw for anything other that static tests")
	set(CMAKE_CXX_FLAGS "-Wno-undef")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2")
elseif(MSVC)
	set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
	set(CMAKE_CXX_FLAGS_DEBUG "/DEBUG")
else()
	set(CMAKE_CXX_FLAGS "-Wno-undef -Wall -Wextra -Wconversion -Wpedantic -Wshadow")
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
	source/Quaternion.cpp
)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Relativty/bin/)
include_directories(AFTER SYSTEM include)

#$<0:> remove the Debug & release folder so the binary is directly in the right folder
# CMAKE_ mean that it is applied to all sub-projects so all libraries should automatically be place in the right folder
if(WIN32)
	if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" )
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/win64/$<0:>)
	else()
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/win32/$<0:>)
	endif()

	set(SOURCES
		source/WindowsSerial.cpp
		${SOURCES}
	)
else()
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}linux64/$<0:>)
	#remove the lib from libdriver_relativty.so
	set(CMAKE_SHARED_LIBRARY_PREFIX "")

	#adding our own implementation of stricmp since gnu doesn't have one
	set(SOURCES
		source/LinuxSerial.cpp
		${SOURCES}
	)
endif()


# add the executable
add_library(driver_relativty SHARED ${SOURCES})

if(MINGW)
	target_link_libraries(driver_relativty ws2_32)
endif()

if(WIN32)
	add_subdirectory(hidapi)
	target_link_libraries(driver_relativty hidapi::hidapi hidapi::include)
	if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
		target_link_libraries(driver_relativty	"${CMAKE_SOURCE_DIR}/libraries/x64/python38.lib")
	else()
		target_link_libraries(driver_relativty	"${CMAKE_SOURCE_DIR}/libraries/Win32/python38.lib")
	endif()
else()
	find_package (Python REQUIRED
		COMPONENTS Interpreter Development)
	target_link_libraries(driver_relativty Python::Python)

	#for linux it's simple and better to use the distribution provided binary (do not use the libusb one)
	find_package(PkgConfig REQUIRED)
	pkg_search_module(HIDAPI REQUIRED hidapi-hidraw)
	target_include_directories(driver_relativty PRIVATE ${HIDAPI_INCLUDE_DIRS})
	target_link_libraries(driver_relativty ${HIDAPI_LIBRARIES})
endif()

set_property(TARGET driver_relativty PROPERTY C_STANDARD 17)
set_property(TARGET driver_relativty PROPERTY CXX_STANDARD 17)
