
#          Copyright Michael Florian Hava.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.21)
project(TEChess)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) # enable virtual folders for projects if supported by the build system/IDE

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# configure C++20
set(CMAKE_CXX_STANDARD 20)          # request C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # enforce requested standard
set(CMAKE_CXX_EXTENSIONS OFF)       # disable compiler specific extensions

add_library(chess-lib STATIC)
	if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"        OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"        OR
	   "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"      OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"      OR
	   "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang"   )
		target_compile_options(chess-lib PUBLIC
			-Wall        # enable "all" warnings
			-Wextra      # enable extra warnings
			-Wpedantic   # enable strict conformance warnings
			-Wconversion # enable warnings for dangerous implicit conversions
			-Werror=vla  # disable support for VLAs
		)
	elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(chess-lib PUBLIC
			/W4          # enable almost all "necessary" warnings
			/permissive- # enforce strict standard compliance
			/JMC         # optimize debugging experience
			/MP          # enable parallel compilation
		)
	else()
		message(WARNING "unknown compiler, no warning flags set!")
	endif()
	file(GLOB_RECURSE SRC "lib/*")
		source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/lib" FILES ${SRC})
	target_sources(chess-lib PRIVATE ${SRC})
	target_include_directories(chess-lib PUBLIC "lib")

add_executable(chess-test)
	file(GLOB_RECURSE SRC "test/*")
		source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/test" FILES ${SRC})
	target_sources(chess-test PRIVATE ${SRC})
	target_link_libraries(chess-test PRIVATE chess-lib)
	find_package(Catch2 CONFIG REQUIRED)
		target_link_libraries(chess-test PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)

add_executable(chess)
	file(GLOB_RECURSE SRC "chess/*")
		source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/chess" FILES ${SRC})
	target_sources(chess PRIVATE ${SRC})
	target_link_libraries(chess PRIVATE chess-lib)
