cmake_minimum_required(VERSION 3.0)
project(Fledermouse VERSION 1.0.0.0)

set(ULTRALEAP_POLLER_SRCS
	  "include/UltraleapPoller.h"
	  "src/UltraleapPoller.cpp")

add_library(ultraleap_poller
	          ${ULTRALEAP_POLLER_SRCS})

target_include_directories(ultraleap_poller
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(ultraleap_poller
	PUBLIC
	LeapSDK::LeapC)
