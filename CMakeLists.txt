cmake_minimum_required(VERSION 3.0)
project(Fledermaus LANGUAGES CXX VERSION 1.0.0.0)

add_subdirectory(mouse_control)

set(Fledermaus_SRCS
    "main.cpp")

add_executable(${PROJECT_NAME}
	             ${Fledermaus_SRCS})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
											mouse_control)
