cmake_minimum_required(VERSION 3.20)
project(SIC)

set(CMAKE_CXX_STANDARD 14)

add_executable(SIC main.cpp utility.cpp pass1.cpp pass2.cpp pass2.h)
