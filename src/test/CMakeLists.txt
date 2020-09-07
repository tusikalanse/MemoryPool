cmake_minimum_required (VERSION 2.8)

project (testmemorypool)

find_library(LIB_GTEST gtest)

list(APPEND LIBS ${LIB_GTEST})

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

add_executable(testmemorypool testmymemorypool.cpp)

target_link_libraries(testmemorypool PUBLIC ${LIBS})