cmake_minimum_required(VERSION 3.16)
project(OGLhangman)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/glfw)
include_directories(include)
include_directories(src)

set(SOURCE_FILES
        main.cpp
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(Google_Tests_run PRIVATE src)

# Linking GLFW and OGL
target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

set(WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/src)
