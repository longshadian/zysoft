cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME zysoft_test)
project(${PROJECT_NAME})

# message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
if(TRUE)
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
endif()

#### cmake模块
set(CMAKE_MODULE_PATH "${CMAKE_HOME_DIRECTORY}/cmake" ${CMAKE_MODULE_PATH})
include(Functions)
include_directories(${CMAKE_HOME_DIRECTORY})

set(SRC_DIR "${CMAKE_HOME_DIRECTORY}/tests/zysoft_test")
util_auto_sources(SRC_CPP_FILES "*.cpp" "RECURSE" "${SRC_DIR}")
util_auto_sources(SRC_H_FILES "*.h" "RECURSE" "${SRC_DIR}")
util_auto_sources(SRC_HPP_FILES "*.hpp" "RECURSE" "${SRC_DIR}")
util_auto_source_group(zysoft_test ${SRC_DIR} ${SRC_CPP_FILES} ${SRC_H_FILES} ${SRC_HPP_FILES})

include_directories(${CMAKE_HOME_DIRECTORY})
include_directories(${CMAKE_HOME_DIRECTORY}/tests/zysoft_test)

#set(CMAKE_CXX_STANDARD 14)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    #add_definitions("/std:c++17")
    add_definitions("/D _WIN32_WINNT=0x0601")
    #add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
    add_definitions("/D NOMINMAX")
    add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

endif()

add_executable(${PROJECT_NAME} ${SRC_CPP_FILES} ${SRC_H_FILES})
target_link_libraries(${PROJECT_NAME} zysoft)
#add_library(${OUTPUT} ${SRC_LIST})
