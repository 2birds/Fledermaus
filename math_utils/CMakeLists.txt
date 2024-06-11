cmake_minimum_required(VERSION 3.0)
project(Fledermouse VERSION 1.0.0.0)

add_library(math_utils INTERFACE)
target_include_directories(math_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)