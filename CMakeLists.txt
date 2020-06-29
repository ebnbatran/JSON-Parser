cmake_minimum_required(VERSION 3.10.0)

project(JSON_Parser)

# we need c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(json)

# the main parser executable
add_executable(parser src/main.cpp)

target_link_libraries(parser PRIVATE JSON)

# the tests executable
add_executable(tests src/tests.cpp)

# we need `-pthread` option for the gtest library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(
    tests PRIVATE 
    JSON 
    gtest PRIVATE Threads::Threads)