cmake_minimum_required(VERSION 3.16)
project(TinyMuduo CXX)

set(CMAKE_CXX_STANDARD 17)

set(CXX_FLAGS
        -g
        -Wall
        -std=c++17
        -D_PTHREADS
        -Wno-unused-parameter
        -O2
        )

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

add_subdirectory(src)
add_subdirectory(example)
