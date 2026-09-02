cmake_minimum_required(VERSION 3.28)

# On Apple Silicon, an x86_64 `cmake` binary running under Rosetta launches
# child processes (including `uname`) translated too, so both CMake's default
# arch detection and a naive `uname -m` report x86_64 even on arm64 hardware.
# `sysctl hw.optional.arm64` queries a hardware capability flag instead, which
# stays accurate regardless of the calling process's own translation state.
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    execute_process(
        COMMAND sysctl -n hw.optional.arm64
        OUTPUT_VARIABLE IS_APPLE_SILICON
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(IS_APPLE_SILICON STREQUAL "1")
        set(CMAKE_OSX_ARCHITECTURES arm64)
    else()
        set(CMAKE_OSX_ARCHITECTURES x86_64)
    endif()
endif()

project(LightSpeedLinearAlgebra CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# LSLASimple: the straightforward reference implementation. Kept buildable so
# its behavior can be compared against the optimized library.
# ---------------------------------------------------------------------------
add_library(LSLASimple SHARED SimpleVersion/LSLASimple.cpp)
target_include_directories(LSLASimple PUBLIC SimpleVersion)

# ---------------------------------------------------------------------------
# LSLA: the main library, where the optimization work happens.
# ---------------------------------------------------------------------------
add_library(LSLA SHARED src/LSLA.cpp)
target_include_directories(LSLA PUBLIC include)
target_link_libraries(LSLA PRIVATE Threads::Threads)

# The two libraries live in separate namespaces (LSLA and LSLASimple), so a
# consumer can link both and compare them side by side.
add_executable(app apps/main.cpp)
target_link_libraries(app PRIVATE LSLA)

enable_testing()

add_executable(vector_tests tests/vector_tests.cpp)
target_link_libraries(vector_tests PRIVATE LSLA LSLASimple)
add_test(NAME vector_tests COMMAND vector_tests)
