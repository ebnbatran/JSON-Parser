cmake_minimum_required(VERSION 3.10.0)

project(JSON_Parser)

# we need c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# the JSON Library
add_library(
    JSON 
    json/json.hpp 
    json/json.cpp 
    json/utility.hpp 
    json/utility.cpp
)

# the main parser executable
add_executable(parser src/main.cpp)
target_link_libraries(parser PUBLIC JSON)

# the tests executable
add_executable(tests src/tests.cpp)

# we need `-pthread` option for the gtest library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(
    tests PUBLIC 
    JSON 
    gtest PRIVATE Threads::Threads)
