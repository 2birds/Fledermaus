cmake_minimum_required(VERSION 3.0)
project(Fledermouse VERSION 1.0.0.0)


set(MOUSE_CONTROL_SRCS
	  "include/MouseControl.h")

if (UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)
  list(APPEND
		   MOUSE_CONTROL_SRCS
			"src/LinuxMouseControl.cpp")
elseif(WIN32)
  list(APPEND
		   MOUSE_CONTROL_SRCS
		   "src/WindowsMouseControl.cpp")
elseif(APPLE)
  list(APPEND
		   MOUSE_CONTROL_SRCS
		   "src/OSXMouseControl.cpp")
endif()

add_library(mouse_control
	          ${MOUSE_CONTROL_SRCS})

target_include_directories(mouse_control
	                         PUBLIC
													 ${CMAKE_CURRENT_SOURCE_DIR}/include)
	          
if (UNIX AND NOT APPLE)
	target_link_libraries(mouse_control
		PRIVATE
		X11)
elseif(APPLE)
    target_link_libraries(mouse_control
	    PRIVATE
		"-framework ApplicationServices")
elseif(WIN32)
endif()
