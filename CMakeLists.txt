cmake_minimum_required(VERSION 3.7.2)
project(server C)

set(CMAKE_C_STANDARD 99)

add_executable(server main.c sub.c sub.h keyValueStore.c keyValueStore.h)