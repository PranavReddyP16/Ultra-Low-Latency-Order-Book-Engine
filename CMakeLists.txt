cmake_minimum_required(VERSION 3.16)
project(HFTOrderBook VERSION 1.0 LANGUAGES CXX)

# Require C++20 for latest optimizations
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type setup
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Detect WSL2
execute_process(
    COMMAND uname -a
    OUTPUT_VARIABLE UNAME_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(UNAME_OUTPUT MATCHES "WSL2")
    message(STATUS "Building on WSL2")
    set(BUILD_ON_WSL2 TRUE)
    add_definitions(-DWSL2_BUILD)
else()
    set(BUILD_ON_WSL2 FALSE)
endif()

# Compiler flags for different build types
set(CMAKE_CXX_FLAGS_DEBUG 
    "-g -O0 -Wall -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE 
    "-O3 -DNDEBUG -march=native -mtune=native -flto -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO 
    "-O2 -g -DNDEBUG -march=native")

# Find required packages
find_package(Threads REQUIRED)

# Optional: Google Test for unit tests
find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    message(STATUS "Google Test found - building tests")
else()
    message(WARNING "Google Test not found - skipping tests")
endif()

# Include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src)

# Only add tests if directory exists and GTest is found
if(GTest_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()