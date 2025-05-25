cmake_minimum_required(VERSION 3.10)
project(HTTP_Server)

add_executable(HTTP_Server src/main.cpp
							src/HTTP_Server.h
							src/HTTP_Server.cpp
							src/Response.h
							src/Response.cpp
							src/Request.h
							src/Request.cpp)
target_link_libraries(HTTP_Server ws2_32 CNet)