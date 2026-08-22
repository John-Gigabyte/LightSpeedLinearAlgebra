cmake_minimum_required(VERSION 3.28)
project(LightSpeedLinearAlgebra CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(LSLA SHARED src/LSLA.cpp)
target_include_directories(LSLA PUBLIC include)

add_executable(app apps/main.cpp)
target_link_libraries(app PRIVATE LSLA)
