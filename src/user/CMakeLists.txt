cmake_minimum_required(VERSION 3.23)

set(project "user")
project(${project})

set(CMAKE_CXX_STANDARD 17)

set(${project}_HEADERS user.h)

set(${project}_SOURCES user.cpp)

set(${project}_SOURCE_LIST ${${project}_HEADERS} ${${project}_SOURCES})

add_library(${project} STATIC ${${project}_SOURCE_LIST})