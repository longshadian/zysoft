CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS On)

set(CMAKE_PROJECT_NAME TcpClient)
project(${CMAKE_PROJECT_NAME})
set(BIN_DIR ${CMAKE_BINARY_DIR}/output)

set(SRC_LIST 
    tcp_client.cpp
        )
		
include_directories(${CMAKE_HOME_DIRECTORY})
    
if(WIN32)
    add_definitions(/std:c++17)
    add_definitions("/D _WIN32_WINNT=0x0601")
    add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
    add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 
    add_definitions("/W4")

elseif(UNIX)
    #ADD_DEFINITIONS(-g -Wall -Wextra -Werror -std=c++17)
    #ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
    #SET(THIRD_LIBRARIES ${THIRD_LIBRARIES} pthread)
    add_definitions(-g -Wall -std=c++17)
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SRC_LIST})

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(${CMAKE_PROJECT_NAME} zysoft Ws2_32)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(${CMAKE_PROJECT_NAME} zysoft pthread)
endif()
