cmake_minimum_required(VERSION 3.14)
project(SPSC_RingBuffer
    VERSION 1.0.0
    LANGUAGES CXX
)

# ----------------------------------------------------------------------
# Global settings
# ----------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)          # use -std=c++17, not -std=gnu++17
set(CMAKE_BUILD_TYPE Release)          # you can override with -DCMAKE_BUILD_TYPE=Debug

# Optimization flags (only for Release builds)
add_compile_options(
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-march=native>
    $<$<CONFIG:Release>:-mtune=native>
    $<$<CONFIG:Release>:-flto>
)

# Strict warnings (optional but recommended)
add_compile_options(
    -Wall -Wextra -Wpedantic
    -Werror=return-type
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wshadow>
)

# ----------------------------------------------------------------------
# Directory layout
# ----------------------------------------------------------------------
set(PROJECT_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ----------------------------------------------------------------------
# Gather source files
# ----------------------------------------------------------------------
set(PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
)
# If you only have main.cpp in the root, you can also do:
# set(PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# ----------------------------------------------------------------------
# Include directories (public for the executable)
# ----------------------------------------------------------------------
include_directories(${PROJECT_INCLUDE_DIR})

# ----------------------------------------------------------------------
# Executable
# ----------------------------------------------------------------------
add_executable(ringbuffer ${PROJECT_SOURCES})

# Make the include directory visible to the target (modern style)
target_include_directories(ringbuffer
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ringbuffer
    PRIVATE
        pthread
)

# ----------------------------------------------------------------------
# Install rules (optional – makes `make install` work)
# ----------------------------------------------------------------------
install(TARGETS ringbuffer
        RUNTIME DESTINATION bin
)

install(DIRECTORY ${PROJECT_INCLUDE_DIR}/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# ----------------------------------------------------------------------
# Testing (optional – enable with -DENABLE_TESTS=ON)
# ----------------------------------------------------------------------
option(ENABLE_TESTS "Build unit tests" OFF)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()