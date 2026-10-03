cmake_minimum_required(VERSION 3.20)

project(linux-mux VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
	"src/*.c"
	"include/*.h"
)

add_executable(linux-mux ${SOURCES})

target_include_directories(
	linux-mux
	PRIVATE
	include
)

target_compile_options(
	linux-mux
	PRIVATE
	-Wall
	-Werror
	-Wextra
	-Wconversion
)
