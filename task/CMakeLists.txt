cmake_minimum_required(VERSION 3.10)
set(CMAKE_PROJECT_NAME "task")

project(${CMAKE_PROJECT_NAME})

set(CMAKE_CXX_STANDART 23 )

add_executable(${CMAKE_PROJECT_NAME} main.cpp task.h)