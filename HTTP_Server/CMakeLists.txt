cmake_minimum_required(VERSION 3.10)
project(HTTP_Server)

add_executable(HTTP_Server src/main.cpp
							src/HTTP_Server.h
							src/HTTP_Server.cpp
							src/Response.h
							src/Response.cpp
							src/Request.h
							src/Request.cpp
							src/HTTPS_Server.h
							src/HTTPS_Server.cpp)
add_custom_command(TARGET HTTP_Server POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/public $<TARGET_FILE_DIR:HTTP_Server>/public)

add_custom_command(TARGET HTTP_Server POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/certificates $<TARGET_FILE_DIR:HTTP_Server>/certificates)

find_package(OpenSSL REQUIRED)

target_link_libraries(HTTP_Server
    PUBLIC ws2_32
    PUBLIC CNet
    PUBLIC OpenSSL::SSL
    PUBLIC OpenSSL::Crypto
)

target_include_directories(HTTP_Server
    PUBLIC ${OPENSSL_INCLUDE_DIR}
)

message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")