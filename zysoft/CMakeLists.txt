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

## zysoft
set(CMAKE_MODULE_PATH "${CMAKE_HOME_DIRECTORY}/cmake" ${CMAKE_MODULE_PATH})

#### cmake模块
include(Functions)
include_directories("C:/vcpkg/installed/x64-windows/include")
include_directories(${CMAKE_HOME_DIRECTORY})

set(SRC_DIR "${CMAKE_SOURCE_DIR}/zysoft")
auto_sources(SRC_FILES "*.cpp" "RECURSE" "${SRC_DIR}")
auto_sources(SRC_HFILES "*.h" "RECURSE" "${SRC_DIR}")
auto_sources(SRC_HPPFILES "*.hpp" "RECURSE" "${SRC_DIR}")
auto_source_group(zysoft ${SRC_DIR} ${SRC_FILES} ${SRC_HFILES} ${SRC_HPPFILES})

#find_package(Boost COMPONENTS filesystem system REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})

add_definitions("/std:c++17")
add_definitions("/D _WIN32_WINNT=0x0601")
add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
#add_definitions("/D _CRT_SECURE_NO_WARNINGS")
add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES} ${SRC_HFILES} ${SRC_HPPFILES})
#add_library(${OUTPUT} ${SRC_LIST})
