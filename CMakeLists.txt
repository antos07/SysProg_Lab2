cmake_minimum_required(VERSION 3.26)
project(SysProg_Lab2 C)

set(CMAKE_C_STANDARD 23)

add_executable(SysProg_Lab2 main.c
        finite_automata.c
        finite_automata.h
)
