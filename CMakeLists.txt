cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/elementary/runtime)


# Add the elementary subdirectory
add_subdirectory(elementary/runtime)

# Source files
set(SRCS
    src/audio.cpp
)

# Library name
set(LIB_NAME myaudiolib)

# Create the dynamic library
add_library(${LIB_NAME} SHARED ${SRCS})

# Set the output name for the library
set_target_properties(${LIB_NAME} PROPERTIES OUTPUT_NAME "myaudiolib" PREFIX "lib" SUFFIX ".dylib")

# Add include directories
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Link any third-party dependencies if needed (example)
# target_link_libraries(${LIB_NAME} some_third_party_lib)
