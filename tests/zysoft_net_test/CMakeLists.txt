CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(tcp_server)
add_subdirectory(tcp_client)

