cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

#add_executable(untitled main.cpp bdd.h bdd.lib)
add_subdirectory (external/buddy bdd)

add_executable(untitled main.cpp)
target_include_directories(untitled PRIVATE "${PROJECT_SOURCE_DIR}/external/buddy/src")
target_link_libraries(untitled bdd)
