cmake_minimum_required(VERSION 3.10)

# Project name
project(Clouds)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress deprecation warnings
add_compile_options(-Wno-deprecated-declarations)

# Add the main executable and necessary source files
add_executable(Clouds
    main.cpp
    camera.cpp
    Cloud.cpp
    camera.h
    Cloud.h
    src/rgba.h
    src/backend.h
    src/stb_image_write.h
)

# Include directories for glm (header-only library)
target_include_directories(Clouds PRIVATE ${CMAKE_SOURCE_DIR}/glm)

# Manually set the compilers to the LLVM ones
set(CMAKE_C_COMPILER /opt/homebrew/opt/llvm/bin/clang)
set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)

# Add OpenMP support
set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
set(OpenMP_C_LIB_NAMES "omp")
set(OpenMP_omp_LIBRARY /opt/homebrew/opt/llvm/lib/libomp.dylib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/opt/llvm/lib")

# Link the OpenMP library
target_link_libraries(Clouds PRIVATE ${OpenMP_omp_LIBRARY})

# Set the build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add optimization flags for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Check for additional dependencies or configurations
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using LLVM Clang from /opt/homebrew/opt/llvm")
