cmake_minimum_required(VERSION 3.0)

project(NTCheck)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(ntcheck src/main.c src/core.c)

