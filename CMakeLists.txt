cmake_minimum_required(VERSION 3.27)
project(sql_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(sql_cpp main.cpp
        sqlite3-wrapper.h
        sqlite3-wrapper.cpp)
target_link_libraries(sql_cpp LINK_PUBLIC sqlite3)
