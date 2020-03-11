cmake_minimum_required(VERSION 3.0.0)

set(This big-int)

project(${This} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(googletest)

set( HEADERS
    "BigInt.hpp"
)
set( SOURCES
    "BigInt.cpp"
)

add_library(${This} STATIC ${SOURCES} ${HEADERS})

add_subdirectory(tests)