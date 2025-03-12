cmake_minimum_required(VERSION 3.10.0)

project(cmage_processing C)

file(GLOB SOURCES "src/main.c"
                  "src/image/*.c"
                  "src/server/*.c"
                  "src/transform/*.c",
                  "src/filters/*.c"
                  "src/utils/*.c")

include_directories("include")

message(SOURCES="${SOURCES}")

# libpng
include_directories("/opt/homebrew/Cellar/libpng/1.6.47/include")
link_directories("/opt/homebrew/Cellar/libpng/1.6.47/lib")

# libmicrohttpd
include_directories("/opt/homebrew/Cellar/libmicrohttpd/1.0.1/include")
link_directories("/opt/homebrew/Cellar/libmicrohttpd/1.0.1/lib")

add_executable(cmage_processing ${SOURCES})

# link libs
target_link_libraries(cmage_processing png)
target_link_libraries(cmage_processing microhttpd)
