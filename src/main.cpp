#include <iostream>
#include <fstream>

#include "../json/json.hpp"

int main(int argc, char *argv[])
{
    if (argc == 1) {
        return -1;
    }

    std::ifstream file{ argv[1] };

    if (!file.is_open()) {
        return -2;
    }     

    file.seekg(0, std::ios::end);
    long long fsize = (long long)file.tellg();
    
    if (fsize == 0) {
        file.close();
        return -3;
    }

    char buffer[fsize];
    file.seekg(0, std::ios::beg);
    file.read(buffer, (std::streamsize)fsize);
    file.close();
    std::string data{ buffer };

    if (data.empty()) {
        return -4;
    }

    JSON::Json json = JSON::Json::fromCppString(data);

    std::cout << "Welcome to JSON Parser" << std::endl;
    std::cout << "C++ 2020" << std::endl;
    std::cout << std::endl;

    std::cout << "Name: " << json["name"] << std::endl;
    std::cout << "Age: " << json["age"] << std::endl;
    std::cout << "Job: " << json["job"] << std::endl;
    std::cout << "Salary: " << json["salary"] << std::endl;
    std::cout << "Favorite Numbers: " << json["favorite_numbers"] << std::endl;
    std::cout << "Siblings: " << json["siblings"] << std::endl;
    std::cout << "Parents: " << std::endl;
    std::cout << json["parents"] << std::endl; 

    return 0;
}
