cmake_minimum_required(VERSION 3.0)
set(CMAKE_PROJECT_NAME zysoft)
message("------------> CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")
project(${CMAKE_PROJECT_NAME})


message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
if(TRUE)
    set(BIN_DIR ${CMAKE_BINARY_DIR}/binaries)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}/debug/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}/debug/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}/debug/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}/release/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}/release/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}/release/lib)
endif()

#### cmake模块
set(CMAKE_MODULE_PATH "${CMAKE_HOME_DIRECTORY}/cmake" ${CMAKE_MODULE_PATH})
include(Functions)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/zysoft")
util_auto_sources(SRC_CPP_FILES "*.cpp" "RECURSE" "${SRC_DIR}")
util_auto_sources(SRC_H_FILES "*.h" "RECURSE" "${SRC_DIR}")
util_auto_sources(SRC_HPP_FILES "*.hpp" "RECURSE" "${SRC_DIR}")
util_auto_source_group(zysoft ${SRC_DIR} ${SRC_CPP_FILES} ${SRC_H_FILES} ${SRC_HPP_FILES})

#find_package(Boost COMPONENTS filesystem system REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_HOME_DIRECTORY})

set(CMAKE_CXX_STANDARD 11)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    #add_definitions("/std:c++11")
    add_definitions("/D _WIN32_WINNT=0x0601")
    #add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
    #add_definitions("/D _CRT_SECURE_NO_WARNINGS")
    add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
endif()

#link_libraries(ws2_32 wsock32)
#add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES} ${SRC_HFILES} ${SRC_HPPFILES})
#target_link_libraries(${project_name}  wsock32)
add_library(${CMAKE_PROJECT_NAME} ${SRC_CPP_FILES} ${SRC_H_FILES} ${SRC_HPP_FILES})
#target_include_directories(zysoft PRIVATE ${CMAKE_SOURCE_DIR}/include)

install(TARGETS zysoft  
        EXPORT MyLibTargets 
        LIBRARY DESTINATION lib  # 动态库安装路径
        ARCHIVE DESTINATION lib  # 静态库安装路径
        RUNTIME DESTINATION bin  # 可执行文件安装路径
        PUBLIC_HEADER DESTINATION include  # 头文件安装路径
    )

#[[
install(FILES ${SRC_H_FILES}
    DESTINATION include
)
#]]

##[[
install(DIRECTORY ${CMAKE_SOURCE_DIR}/zysoft/zysoft
    DESTINATION "include"
    FILES_MATCHING PATTERN "*.h"
)
#]]

