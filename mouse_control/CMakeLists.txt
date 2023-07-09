cmake_minimum_required(VERSION 3.0)
project(Fledermouse VERSION 1.0.0.0)

find_package(X11 REQUIRED)

set(MOUSE_CONTROL_SRCS
	  "include/MouseControl.h")

if (UNIX)
  list(APPEND
		   MOUSE_CONTROL_SRCS
			"src/LinuxMouseControl.cpp")
elseif(WIN32)
  list(APPEND
		   MOUSE_CONTROL_SRCS
		   "src/WindowsMouseControl.cpp")
endif()

add_library(mouse_control
	          ${MOUSE_CONTROL_SRCS})

target_include_directories(mouse_control
	                         PUBLIC
													 ${CMAKE_CURRENT_SOURCE_DIR}/include)
	          
if (UNIX)
	target_link_libraries(mouse_control
		PRIVATE
		X11)
elseif(WIN32)
endif()
