cmake_minimum_required(VERSION 3.10)
project(CNet)

add_library(CNet STATIC
    	src/IncludeFile.h
		src/Network.h
		src/Network.cpp
)
target_include_directories(CNet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)