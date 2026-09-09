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

# Benchmarks and "optimized vs simple" comparisons are meaningless in an
# unoptimized build, and plain `cmake -S . -B build` leaves CMAKE_BUILD_TYPE
# empty on single-config generators (Makefiles/Ninja). Default to Release so
# a fresh build is actually optimized unless the caller asks otherwise.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

find_package(Threads REQUIRED)
include(CheckIPOSupported)
include(CheckCXXCompilerFlag)
include(GenerateExportHeader)

# Interprocedural optimization (LTO) lets the compiler inline and optimize
# across translation units within each library. Only enabled for optimized
# configs: it slows down Debug builds and can make debugging harder there.
check_ipo_supported(RESULT ipoSupported OUTPUT ipoError)
if(ipoSupported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
endif()

# Baseline x86-64 (what CMAKE_CXX_FLAGS defaults to) targets SSE2 only, so
# -O3 alone will not autovectorize with anything wider. -march=x86-64-v3
# raises the floor to AVX2/FMA, which is supported by essentially every x86_64
# CPU sold since ~2013. This is opt-out (not -march=native) so the resulting
# binaries stay portable to any machine in that range, rather than only the
# machine that built them. Disable with -DLSLA_ENABLE_ARCH_TUNING=OFF to build
# fully portable (SSE2-only) binaries, e.g. for distribution to unknown/older
# hardware.
option(LSLA_ENABLE_ARCH_TUNING "Enable -march=x86-64-v3 (AVX2/FMA) code generation" ON)
if(LSLA_ENABLE_ARCH_TUNING)
    check_cxx_compiler_flag("-march=x86-64-v3" LSLA_HAS_ARCH_V3)
    if(LSLA_HAS_ARCH_V3)
        add_compile_options($<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-march=x86-64-v3>)
    endif()
endif()

# Hide symbols by default so only the explicitly annotated public API (via
# generate_export_header, below) is exported from each shared library. This
# lets the compiler devirtualize/inline more aggressively within a library
# and avoids -fPIC's indirect-call overhead for internal calls to functions
# that would otherwise default to being interposable/exported.
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# ---------------------------------------------------------------------------
# LSLASimple: the straightforward reference implementation. Kept buildable so
# its behavior can be compared against the optimized library.
# ---------------------------------------------------------------------------
add_library(LSLASimple SHARED SimpleVersion/LSLASimple.cpp)
target_include_directories(LSLASimple PUBLIC SimpleVersion ${CMAKE_CURRENT_BINARY_DIR})
generate_export_header(LSLASimple
    BASE_NAME LSLASimple
    EXPORT_FILE_NAME LSLASimple_export.h)

# ---------------------------------------------------------------------------
# LSLA: the main library, where the optimization work happens.
# ---------------------------------------------------------------------------
add_library(LSLA SHARED src/LSLA.cpp)
target_include_directories(LSLA PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(LSLA PRIVATE Threads::Threads)
generate_export_header(LSLA
    BASE_NAME LSLA
    EXPORT_FILE_NAME LSLA_export.h)

# The two libraries live in separate namespaces (LSLA and LSLASimple), so a
# consumer can link both and compare them side by side.
add_executable(app apps/main.cpp)
target_link_libraries(app PRIVATE LSLA)

enable_testing()

add_executable(vector_tests tests/vector_tests.cpp)
target_link_libraries(vector_tests PRIVATE LSLA LSLASimple)
add_test(NAME vector_tests COMMAND vector_tests)
