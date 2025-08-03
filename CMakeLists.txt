cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_COMPILER clang)

set(CMAKE_GENERATOR Ninja)

project(codecrafters-shell)

set(CMAKE_C_STANDARD 23)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -fno-inline -fno-builtin -g -O0 -Wall -Werror")

#SET(CMAKE_BUILD_TYPE Debug)

#add_executable(shell src/main.c src/builtins.c src/base/arena.c src/base/string.c src/stringplayground.c)
add_executable(shell src/builtins.c src/base/arena.c src/base/string.c src/main.c)
