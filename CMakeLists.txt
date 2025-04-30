cmake_minimum_required(VERSION 3.10)
set(ProjectName CNetwork)
project(${ProjectName})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(Client)
add_subdirectory(CNet)
add_subdirectory(Server)