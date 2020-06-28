# JSON-Parser
C++ library to parse JSON files

## Description
This is a small C++ library that parses JSON files into a data structure caled Json
It is made as an effort to re-invent the wheel and to better understand how parsing
works.

## Building
### Buiding "parser" executable
Using gcc compiler write down:
`g++ -std=c++17 -o parser main.cpp json.cpp utility.cpp`
### Building "tests" executable
You need to have GoogleTests framework installed where your compiler can
find them.

Then, using gcc compiler write down:
`g++ -std=c++17 -o tests tests.cpp json.cpp utility.cpp -lgtest -pthread`
