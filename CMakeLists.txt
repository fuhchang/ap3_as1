cmake_minimum_required(VERSION 3.3)
project(aph_ex1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    date.c
    date.h
    tldlist.c
    tldlist.h
    tldmonitor.c
    small.txt)

add_executable(aph_ex1 ${SOURCE_FILES})