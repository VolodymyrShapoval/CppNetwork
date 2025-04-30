cmake_minimum_required(VERSION 3.10)
project(Client)

add_executable(Client src/main.cpp)
target_link_libraries(Client ws2_32 CNet)