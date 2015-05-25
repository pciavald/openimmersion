cmake_minimum_required (VERSION 2.6)
project (Tutorial)
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

link_libraries(m)
include_directories("${PROJECT_BINARY_DIR}")

add_executable(Tutorial tutorial.c)

