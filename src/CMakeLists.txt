cmake_minimum_required(VERSION 3.15)
project(os_lab_4)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
add_executable(os_lab_4 main.c c_queue.h c_queue.c)
