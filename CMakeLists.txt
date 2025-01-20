cmake_minimum_required(VERSION 3.26)
project(student.crow)

set(CMAKE_CXX_STANDARD 20)

include_directories(/usr/local/include)
include_directories(/opt/homebrew/Cellar/asio/1.30.2/include)
include_directories(/Users/lemerleau/softwares/Crow/include)
include_directories(/opt/homebrew/opt/sqlite/include)
include_directories(/opt/homebrew/Cellar/boost/1.87.0/include)


add_executable(student.crow main.cpp)

target_link_libraries(student.crow pthread)
target_link_libraries(student.crow /opt/homebrew/opt/sqlite/lib/libsqlite3.dylib)
