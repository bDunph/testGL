cmake_minimum_required(VERSION 3.15)

project(test)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED) 
#find_library(GLEW_DYLIB GLEW "/usr/local/Cellar/glew/2.1.0/lib")
#if(NOT GLEW_DYLIB)
#	 message(FATAL_ERROR "Glew not found")
#endif()
#add_library(Glew_target SHARED IMPORTED)
#set_property(TARGET Glew_target PROPERTY IMPORTED_LOCATION "/usr/local/Cellar/glew/2.1.0/lib/libGLEW.dylib")
#set_property(TARGET Glew_target PROPERTY IMPORTED_IMPLIB ${GLEW_DYLIB})

#include_directories(
#	"/usr/local/Cellar/glew/2.1.0/include/"	
#	)

add_executable(test main.cpp)
target_link_libraries(test GLEW::GLEW glfw OpenGL::GL) 
