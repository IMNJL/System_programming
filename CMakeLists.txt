cmake_minimum_required(VERSION 3.25)
project(System_programming)

set(CMAKE_STANDARD 17)

add_executable(System_programming lab1_processor_info/main.c exec/exec.c llll.c pid/main.c
        signals/signals.c)
