cmake_minimum_required(VERSION 3.0.0)

set(This BigIntTests)

set(SOURCES 
    BigIntTests.cpp
)

add_executable(${This} ${SOURCES})

target_compile_options(${This} PRIVATE 
    "-Wall"
    "-Wextra"
)

target_link_libraries(${This} PUBLIC 
    gtest_main #target provided by gtest
    big-int #main library
)

add_test(
    NAME ${This}
    COMMAND ${This}
)
