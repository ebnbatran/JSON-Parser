# the JSON Library
add_library(
    JSON 
    json.hpp 
    json.cpp 
    utility.hpp 
)

target_include_directories(JSON PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
