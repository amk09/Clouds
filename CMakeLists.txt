cmake_minimum_required(VERSION 3.10)
project(Clouds)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Add the executable
add_executable(cloud main.cpp camera.cpp terrain.cpp Cloud.cpp)

# Link OpenMP
target_link_libraries(cloud OpenMP::OpenMP_CXX)

# Suppress deprecated declarations warnings
target_compile_options(cloud PRIVATE -Wno-deprecated-declarations)