cmake_minimum_required(VERSION 3.0)

project(rc-string CXX)

add_library(rc_string src/rc-string.cpp)
target_include_directories(rc_string PUBLIC include)

add_executable(rc_string_example example/example.cpp)
target_link_libraries(rc_string_example PRIVATE rc_string)