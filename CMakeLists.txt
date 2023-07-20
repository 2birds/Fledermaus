cmake_minimum_required(VERSION 3.0)
project(Fledermaus LANGUAGES CXX VERSION 1.0.0.0)

if (WIN32)
   set(ULTRALEAP_PATH_ROOT "$ENV{ProgramFiles}/Ultraleap")
elseif (APPLE)
   set(ULTRALEAP_PATH_ROOT "/Library/Application\ Support/Ultraleap/LeapSDK/lib/cmake/LeapSDK")
else()
   # Linux and other OSs
   set(ULTRALEAP_PATH_ROOT "")
endif()

find_package(LeapSDK
     5
     REQUIRED
     PATHS
	 	"${ULTRALEAP_PATH_ROOT}")

	file(COPY "${ULTRALEAP_PATH_ROOT}/LeapSDK/include/LeapC.h"
		DESTINATION "${PROJECT_SOURCE_DIR}/ultraleap_poller/include/")

if (UNIX)    
    find_package(Threads REQUIRED)    
endif (UNIX)
add_subdirectory(mouse_control)
add_subdirectory(ultraleap_poller)

set(Fledermaus_SRCS
    "main.cpp")

add_executable(${PROJECT_NAME}
	             ${Fledermaus_SRCS})

target_link_libraries(${PROJECT_NAME}
     PUBLIC
     LeapSDK::LeapC
     PRIVATE
     mouse_control
     ultraleap_poller)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	"${ULTRALEAP_PATH_ROOT}/LeapSDK/lib/x64/LeapC.dll"
	${CMAKE_BINARY_DIR})

