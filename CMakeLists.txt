cmake_minimum_required(VERSION 3.0)

project(NTCheck)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)

# https://stackoverflow.com/a/52887900
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(ntcheck ${all_SRCS})

