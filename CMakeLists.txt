cmake_minimum_required(VERSION 2.6.0)

project(cmage_processing C)

file(GLOB SOURCES "src/*.c")
include_directories(include)
add_executable(cmage_processing ${SOURCES})