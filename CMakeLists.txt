cmake_minimum_required(VERSION 3.20)
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
     REQUIRED
     PATHS
	 	"${ULTRALEAP_PATH_ROOT}")

	# 	file(COPY "${ULTRALEAP_PATH_ROOT}/LeapSDK/include/LeapC.h"
	# 		DESTINATION "${PROJECT_SOURCE_DIR}/ultraleap_poller/include/")

if (UNIX)    
    find_package(Threads REQUIRED)    
endif (UNIX)
add_subdirectory(math_utils)
add_subdirectory(mouse_control)
add_subdirectory(ultraleap_poller)

set(Fledermaus_SRCS
    "main.cpp")

add_executable(${PROJECT_NAME}
	             ${Fledermaus_SRCS})

target_include_directories(${PROJECT_NAME}
                           PRIVATE
                           "rapidjson/include")

set(link_libraries
    math_utils
    mouse_control
    ultraleap_poller)

if(WIN32)
    list(APPEND link_libraries shlwapi)
endif(WIN32)

target_link_libraries(${PROJECT_NAME}
     PUBLIC
     LeapSDK::LeapC
     PRIVATE
     ${link_libraries})

get_target_property(
	LEAPC_IMPORTED_CONFIG
	LeapSDK::LeapC
	IMPORTED_CONFIGURATIONS)

get_target_property(
	LEAPC_SHARED_LIB_PATH
	LeapSDK::LeapC
	IMPORTED_LOCATION_${LEAPC_IMPORTED_CONFIG})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${LEAPC_SHARED_LIB_PATH}
	$<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${PROJECT_SOURCE_DIR}/fledermaus_config.json
	$<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${PROJECT_SOURCE_DIR}/LICENSE
	$<TARGET_FILE_DIR:${PROJECT_NAME}>)
