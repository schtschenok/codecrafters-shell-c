cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_GENERATOR Ninja)

project(codecrafters-shell)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -g -O1 -Wall -Werror")

add_executable(shell src/build.c)
