cmake_minimum_required	(VERSION 2.6)
project					(OpenImmersion)
set						(VERSION_MAJOR 0)
set						(VERSION_MINOR 1)

set(CMAKE_CXX_FLAGS "
	-Wall
	-Wextra
	-Werror
	")

include_directories(
	"${PROJECT_BINARY_DIR}"
	)

link_directories(
	)

add_executable(			server
	src/server.c
	)
target_link_libraries(	server
	)
