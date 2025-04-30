cmake_minimum_required(VERSION 3.10)
project(Server)

add_executable(Server src/main.cpp)
target_link_libraries(Server ws2_32 CNet)