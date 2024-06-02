cmake_minimum_required(VERSION 3.23)

set(project "commands")
project(${project})

set(CMAKE_CXX_STANDARD 17)

set(${project}_HEADERS commands.h)

set(${project}_SOURCES commands.cpp)

set(${project}_SOURCE_LIST ${${project}_HEADERS} ${${project}_SOURCES})

add_library(${project} STATIC ${${project}_SOURCE_LIST})

target_link_libraries(${project} user)

target_link_libraries(${project} dataBase)

target_link_libraries(${project} keybords)