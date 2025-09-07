cmake_minimum_required(VERSION 4.0)

set(CMAKE_C_COMPILER clang)

set(CMAKE_GENERATOR Ninja)

project(codecrafters-shell)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -fno-inline -fno-builtin -g -O0 -Wall -Werror -Wno-unused-function")

#SET(CMAKE_BUILD_TYPE Debug)

add_executable(shell src/main.c)
