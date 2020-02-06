cmake_minimum_required(VERSION 3.10)
project(Chip8Vm)

set(CMAKE_CXX_STANDARD 14)

add_executable(Chip8Vm main.cpp Chip8.cpp Chip8.h)