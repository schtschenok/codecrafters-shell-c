cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_COMPILER clang)

set(CMAKE_GENERATOR Ninja)

project(codecrafters-shell)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -fsanitize=address,undefined \
    -fno-omit-frame-pointer \
    -mno-omit-leaf-frame-pointer \
    -fno-inline \
    -g -O0 \
    -Wall -Werror \
    -Wno-unused-function \
    -Wno-unused-variable")

add_executable(shell src/main.c)
