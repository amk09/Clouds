cmake_minimum_required(VERSION 3.10)

# Project name
project(Clouds)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wno-deprecated-declarations)


# Add the main executable and necessary source files
add_executable(Clouds main.cpp
    camera.cpp
    camera.h
    rgba.h)

# Include the glm headers
# Assuming glm is a header-only library located in the glm directory
target_include_directories(Clouds PRIVATE ${CMAKE_SOURCE_DIR}/glm)

# Optionally enable optimizations for better performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
