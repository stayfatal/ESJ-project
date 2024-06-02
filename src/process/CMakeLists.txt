cmake_minimum_required(VERSION 3.23)

set(project "process")
project(${project})

set(CMAKE_CXX_STANDARD 17)

set(${project}_HEADERS process.h)

set(${project}_SOURCES process.cpp)

set(${project}_SOURCE_LIST ${${project}_HEADERS} ${${project}_SOURCES})

add_library(${project} STATIC ${${project}_SOURCE_LIST})

target_link_libraries(${project} dataBase)

target_link_libraries(${project} user)

target_link_libraries(${project} keybords)