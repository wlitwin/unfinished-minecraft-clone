cmake_minimum_required(VERSION 3.8)

project(cubes)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -DGL_GLEXT_PROTOTYPES -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

IF (NOT CMAKE_BUILD_TYPE )
    SET ( CMAKE_BUILD_TYPE Debug )
ENDIF()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -g")

# the -I flag in gcc
include_directories(
  ${PROJECT_SOURCE_DIR}/include/
) 

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(Threads REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIR} ${GLM_INCLUDE_DIRECTORIES}
    ${GLFW3_INCLUDE_DIRECTORIES} )

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

FILE(GLOB LIB_SOURCES lib/*.cpp)
FILE(GLOB TEST_SOURCES test/*.cpp)

set(APP_SOURCES src/main.cpp)
set(DEP_LIBS ${OPENGL_LIBRARY} glfw ${GLEW_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

# creates ./build/bin/runner
add_executable(${PROJECT_NAME} ${APP_SOURCES})

# shared library
set(LIBRARY_NAME libcubes)
add_library(${LIBRARY_NAME} STATIC ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME} ${DEP_LIBS})

# Create test library
add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests ${LIBRARY_NAME} ${DEP_LIBS})
