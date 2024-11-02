cmake_minimum_required(VERSION 3.10)
set(CMAKE_PROJECT_NAME "ipv4")

 project(${CMAKE_PROJECT_NAME})

 set(CMAKE_CXX_STANDARD 23)

 add_executable(${CMAKE_PROJECT_NAME} main.cpp ../mtest/tests.h)

