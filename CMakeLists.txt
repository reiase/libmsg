CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

PROJECT(libmsg)
SET(PROJECT_VERSION 0.0.1 CXX)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

# 3rdparty
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
INCLUDE(FetchContent)
INCLUDE(cmake/3rd_catch2.cmake)
INCLUDE(cmake/3rd_libuv.cmake)
INCLUDE(cmake/3rd_ragel.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)