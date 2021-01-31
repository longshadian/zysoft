cmake_minimum_required(VERSION 3.0)

set(CMAKE_PROJECT_NAME zysoft)
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

set(CMAKE_CXX_EXTENSIONS false)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    #add_definitions("/std:c++17")
    add_definitions("/D _WIN32_WINNT=0x0601")
    add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
    #add_definitions("/D _CRT_SECURE_NO_WARNINGS")
    add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

endif()

add_library(${CMAKE_PROJECT_NAME} ${SRC_CPP_FILES} ${SRC_H_FILES} ${SRC_HPP_FILES})

install(TARGETS zysoft  DESTINATION lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/zysoft
    DESTINATION "include"
    FILES_MATCHING REGEX  "[^/.]+.(h|hpp)$"
)
