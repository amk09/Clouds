cmake_minimum_required(VERSION 3.10)

# Project name
project(GLMExample)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the main executable
add_executable(GLMExample main.cpp)

# Include the glm directory
add_subdirectory(glm)

# Optionally enable optimizations for better performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")