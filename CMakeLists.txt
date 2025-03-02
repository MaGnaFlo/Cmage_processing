cmake_minimum_required(VERSION 2.6.0)

project(cmage_processing C)

file(GLOB SOURCES "src/*.c")
include_directories(include)

include_directories("/opt/homebrew/Cellar/libpng/1.6.47/include")
link_directories("/opt/homebrew/Cellar/libpng/1.6.47/lib")


add_executable(cmage_processing ${SOURCES})

target_link_libraries(cmage_processing png)