cmake_minimum_required(VERSION 3.8)
project(Checkers)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES checkers.cpp checkers.h)
add_executable(Checkers ${SOURCE_FILES})