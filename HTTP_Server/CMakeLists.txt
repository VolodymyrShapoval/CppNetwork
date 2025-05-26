cmake_minimum_required(VERSION 3.10)
project(HTTP_Server)

add_executable(HTTP_Server src/main.cpp
							src/HTTP_Server.h
							src/HTTP_Server.cpp
							src/Response.h
							src/Response.cpp
							src/Request.h
							src/Request.cpp)
add_custom_command(TARGET HTTP_Server POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/public $<TARGET_FILE_DIR:HTTP_Server>/public)
target_link_libraries(HTTP_Server ws2_32 CNet)