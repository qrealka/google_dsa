cmake_minimum_required(VERSION 3.15)
project(google_dsa)

set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS ON)

add_executable(hash_set hash_set.cpp)