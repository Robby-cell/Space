cmake_minimum_required(VERSION 3.28)

project(Space)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_library(Space INTERFACE)
include_directories(Space INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(tests)
