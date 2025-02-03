cmake_minimum_required(VERSION 3.10)
 project(alloc)

 set(CMAKE_CXX_STANDARD 17)

 

 set_property(GLOBAL PROPERTY USE_FOLDERS ON)

 add_executable(alloc main.cpp alloc.h)

