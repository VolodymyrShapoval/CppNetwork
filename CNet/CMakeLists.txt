cmake_minimum_required(VERSION 3.10)
project(CNet)

add_library(CNet STATIC
    	src/IncludeFile.h
		src/Network.h
		src/Network.cpp
		src/SocketHandle.h
		src/PResult.h
		src/IPVersion.h
		src/Socket.h
		src/Socket.cpp
		src/SocketOption.h
		src/IPEndpoint.h
		src/IPEndpoint.cpp
		src/Constants.h
)
target_include_directories(CNet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)