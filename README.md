# JSON-Parser
C++ library to parse JSON files

## Description
This is a small C++ library that parses JSON files into a data structure caled Json
It is made as an effort to re-invent the wheel and to better understand how parsing
works.

## Building
You need to have GoogleTest Framework installed where your compiler can find it. 
<br>
The compiler has to be supporting C++17.
<br>
Then:
<br>
`mkdir build`
<br>
`cd build`
<br>
`cmake ..`
<br>
`cmake --build .`
<br>
<br>
### This gives you:
1. **libJSON.a**: the static JSON library
2. **parser**: the main app
3. **tests**: to run all tests

## Running "parser"
Specify `../test.json` as an argument to give the program access to the test JSON
file.
