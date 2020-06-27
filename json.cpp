#include <cstdlib>

#include "json.hpp"

namespace JSON {

    const char *WrongTypeException::what() const noexcept {
        return "Attempt to perform an operation on the wrong Json Type";
    }

    Json::Json() {
        type = Type::Invalid;
    }

    Json::Json(const Type &type) 
        : type(type)
    {
        switch (type) {
            case Type::Boolean: {
                value.boolean = false;
            } break;

            case Type::Integer: {
                value.integer = 0;
            } break;

            case Type::FloatingPoint: {
                value.floatingPoint = 0.0;
            } break;

            case Type::String: {
                value.string = new std::string();
            } break;

            case Type::Array: {
                value.array = new std::vector<Json *>();
            } break;

            case Type::Object: {
                value.object = new std::unordered_map<std::string, Json *>();
            } break;
        }
    }

    Json::Json(Json *other) {
        type = other->type;

        switch (type) {
            case Type::Boolean: {
                value.boolean = other->value.boolean;
            } break;

            case Type::Integer: {
                value.integer = other->value.integer;
            } break;

            case Type::FloatingPoint: {
                value.floatingPoint = other->value.floatingPoint;
            } break;

            case Type::String: {
                const auto otherString = *other->value.string;
                value.string = new std::string(otherString);
            } break;

            case Type::Array: {
                const auto otherArray = *other->value.array;
                value.array = new std::vector<Json *>(otherArray);
            } break;

            case Type::Object: {
                const auto otherObject = *other->value.object;
                value.object = new std::unordered_map<std::string, Json *>(otherObject);
            } break;
        }
    }

    Json::~Json() {
        // do nothing
    }

    Json *Json::parseBoolean(const std::string &input) {
        Json *json = new Json();
        char firstChar = input[0];

        switch (firstChar) {
            case 't': {
                if (
                    (input[1] == 'r') &&
                    (input[2] == 'u') &&
                    (input[3] == 'e')
                ) {
                    json->type = Type::Boolean;
                    json->value.boolean = true;
                }
            } break;

            case 'f': {
                if (
                    (input[1] == 'a') &&
                    (input[2] == 'l') &&
                    (input[3] == 's') &&
                    (input[4] == 'e')
                ) {
                    json->type = Type::Boolean;
                    json->value.boolean = false;
                }
            } break;
        }

        return json;
    }

    Json *Json::parseNull(const std::string &input) {
        Json *json = new Json();
        char firstChar = input[0];
        
        if (firstChar == 'n') {
            if (
                (input[1] == 'u') &&
                (input[2] == 'l') &&
                (input[3] == 'l')
            ) {
                json->type = Type::Null;
            }
        } 

        return json;
    }

    Json *Json::parseInteger(const std::string &input) {
        Json *json = new Json();
        long long result = 0;
        bool negative = false;

        for (const char &c : input) {

            if (c == '-') {
                negative = true;
                continue;
            }                
            
            long long n = JSON::Utility::ctoll(c);

            if (c == '0') {
                n = 0;
            } else if (n == (long long)NAN) {
                return json;
            }

            result *= 10;
            result += n;
        }

        if (negative) {
            result *= -1;
        }

        json->type = Type::Integer;
        json->value.integer = result;    

        return json;
    }

    Json *Json::parseFloatingPoint(const std::string &input) {
        Json *json = new Json();
        long double result = 0.0;
        long double powerValue = 0.0;
        long double d = 1.0; 
        
        int state = 0;
        
        bool negative = false;
        bool power = false;
        bool negPower = false;
        bool floatPower = false;

        for (const auto &c : input) {
            
            // convert char to long double
            long double n = JSON::Utility::ctold(c);

            if (c == '0') {
                n = 0.0;
            } else if (n == (long double)NAN) {
                return json;
            }

            // parse
            switch (state) {
                case 0: { // int part
                    if (c == '-') {
                        negative = true;
                        continue;
                    } else if (c == '.') {
                        state = 1;
                        d = 1.0;
                        continue;
                    } else if (c == 'e') {
                        state = 2;
                        power = true;
                        continue;
                    } else {
                        result *= 10;
                        result += n;
                    }
                } break;

                case 1: { // float part
                    if (c == 'e') {
                        state = 2;
                        power = true;
                        continue;
                    } else {
                        result += n / (10.0 * d);
                        d *= 10.0;
                    }
                } break;

                case 2: { // power part
                    if (c == '-') {
                        negPower = true;
                        continue;
                    } else if (c == '.') {
                        floatPower = true;
                        d = 1.0;
                        continue;
                    }

                    if (!floatPower) { // integral part of the power
                        powerValue *= 10;
                        powerValue += n;
                    } else { // float part of the power
                        powerValue += n / (10.0 * d);
                        d *= 10.0;
                    }
                } break;
            }
        }
        
        if (negative) {
            result *= -1;
        }
        
        json->type = Type::FloatingPoint;

        if (!power) {
            json->value.floatingPoint = result;
        } else {
            if (negPower) {
                powerValue *= -1;
            }

            json->value.floatingPoint = (long double)std::pow(result, powerValue);
        }

        return json;
    }

    Json *Json::parseString(const std::string &input) {
        Json *json = new Json();
        bool foundEndQoute = false;
        std::string extractedString = "";

        for (int i = 1; i < input.size(); ++i) {
            
            const char &c = input.at(i);

            if (c == '"') {
                foundEndQoute = true;
                break;
            }

            extractedString.push_back(c);
        }

        if (!foundEndQoute) {
            return json;
        }

        json->type = Type::String;
        json->value.string = new std::string(extractedString);
        
        return json;
    }

    Json *Json::parseArray(const std::string &input) {
        Json *json = new Json();
        bool foundEndBracket = false;
        bool insideArray = false;
        int state = 0;
        int openBrackets = 0;
        int closeBrackets = 0;
        int openBrace = 0;
        int closeBrace = 0;
        std::string element = "";
        std::vector<std::string> elements;

        for (int i = 1; i < input.size(); ++i) {

            const auto &c = input.at(i);

            switch (state) {
                case 0: { // just element
                    //[[7], [8], [9, [10]]]
                    if (c == ',') {
                        elements.push_back(element);
                        element.clear();
                    } else if (c == '"') {
                        state = 1;
                        element.push_back(c);
                    } else if (c == '[') {
                        openBrackets = 0;
                        closeBrackets = 0;
                        state = 2;
                        element.push_back(c);
                    } else if (c == ']') {
                        foundEndBracket = true;
                        elements.push_back(element);
                        break;
                    } else if (c == '{') {
                        state = 3;
                        openBrace = 0;
                        closeBrace = 0;
                        element.push_back(c);
                    } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                        // do nothing
                    } else {
                        element.push_back(c);
                    }
                } break;

                case 1: { // string double-quote mark
                    if (c == '"') {
                        state = 0;
                    } 

                    element.push_back(c);
                } break;

                case 2: { // opening bracket
                    if (c == '[') {
                        openBrackets++;
                    } else if (c == ']') {
                        closeBrackets++;
                        if (closeBrackets - openBrackets == 1) {
                            state = 0;
                        }
                    }

                    element.push_back(c);
                } break;
                
                case 3: { // opening brace
                    if (c == '{') {
                        openBrace++;
                    } else if (c == '}') {
                        closeBrace++;
                        if (closeBrace - openBrace == 1) {
                            state = 0;
                        }
                    }

                    element.push_back(c);
                } break;
            }  
        }

        if (!foundEndBracket) {
            return json;
        }

        auto jsonArray = new std::vector<Json *>();

        for (const auto &e : elements) {
            
            Json *value = nullptr;
            
            switch (e.front()) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '-': {
                    int pointPos = e.find_first_of(".eE");

                    if (pointPos == std::string::npos) {
                        value = parseInteger(e);
                    } else {
                        value = parseFloatingPoint(e);
                    }
                    
                    jsonArray->push_back(value);
                } break;

                case '"': {
                    value = parseString(e);
                    jsonArray->push_back(value);
                } break;

                case 'T':
                case 'F':
                case 't':
                case 'f': {
                    value = parseBoolean(e);
                    jsonArray->push_back(value);
                } break;

                case 'N':
                case 'n': {
                    value = parseNull(e);
                    jsonArray->push_back(value);
                } break;

                case '[': {
                    value = parseArray(e);
                    jsonArray->push_back(value);
                } break;

                case '{': {
                    value = parseObject(e);
                    jsonArray->push_back(value);
                } break;
            }
        }
        
        json->type = Type::Array;
        json->value.array = jsonArray;

        return json;
    }

    Json *Json::parseObject(const std::string &input) {
        // loop through input and populate a vector of pairs of elements
        Json *json = new Json();
        
        std::vector<std::pair<std::string, std::string>> members;
        std::pair<std::string, std::string> pair;
        std::string name = "";
        std::string content = "";
        
        int state = 0;
        int openBracket = 0;
        int closeBracket = 0;
        int openBrace = 0;
        int closeBrace = 0;
        
        bool memberName = true;
        bool foundEndBrace = false;

        for (int i = 1; i < input.size(); ++i) {
            const auto &c = input.at(i);

            switch (state) {
                
                case 0: { // just element;
                    
                    switch (c) {
                        case '"': {
                            state = 1;
                            
                            if (!memberName) {
                                content.push_back(c);
                            }
                        } break;

                        case ':': {
                            memberName = false;
                        } break;

                        case ',': {
                            pair.first = name;
                            pair.second = content;
                            members.push_back(pair);
                            
                            pair.first.clear();
                            pair.second.clear();
                            name.clear();
                            content.clear();

                            openBracket = 0;
                            closeBracket = 0;
                            openBrace = 0;
                            closeBrace = 0;
                            
                            memberName = true;
                        } break;

                        case '[': {
                            if (!memberName) {
                                state = 2;
                                content.push_back(c);
                                
                                openBracket = 0;
                                closeBracket = 0;
                            }
                        } break;

                        case '{': {
                            if (!memberName) {
                                state = 3;
                                content.push_back(c);
                                
                                openBrace = 0;
                                closeBrace = 0;
                            }
                        } break;

                        case '}': {
                            if (!memberName) {
                                pair.first = name;
                                pair.second = content;
                                members.push_back(pair);
                                
                                pair.first.clear();
                                pair.second.clear();
                                name.clear();
                                content.clear();
                                
                                openBracket = 0;
                                closeBracket = 0;
                                openBrace = 0;
                                closeBrace = 0;
                                
                                memberName = true;
                                foundEndBrace = true;
                                
                                break;
                            }
                        } break;

                        case ' ':
                        case '\t':
                        case '\n':
                        case '\r': {
                            // do nothing
                        } break;

                        default: {
                            if (memberName) {
                                name.push_back(c);
                            } else {
                                content.push_back(c);
                            }
                        } break;
                    }
                } break;

                case 1: { // string double-quote mark
                    if (memberName) {
                        if (c != '"') {
                            name.push_back(c);
                        } else {
                            memberName = false;
                            state = 0;
                        }
                    } else {
                        content.push_back(c);

                        if (c == '"') {
                            state = 0;
                        }
                    }
                } break;

                case 2: { // array open bracket
                    if (!memberName) {
                        content.push_back(c);

                        if (c == '[') {
                            openBracket++;
                        } else if (c == ']') {
                            closeBracket++;

                            if (closeBracket - openBracket == 1) {
                                state = 0;
                            }
                        }
                    }
                } break;

                case 3: { // object open brace
                    if (!memberName) {
                        content.push_back(c);

                        if (c == '{') {
                            openBrace++;
                        } else if (c == '}') {
                            closeBrace++;

                            if (closeBrace - openBrace == 1) {
                                state = 0;
                            }
                        }
                    }
                } break;
            }
        }
        
        // parse each value and poplulate a Json Object
        if (!foundEndBrace) {
            return json;
        }

        auto *object = new std::unordered_map<std::string, Json *>();

        for (const auto &member : members) {
            Json *value = nullptr;
            
            switch (member.second.front()) {
                case 'n': {
                    value = parseNull(member.second);
                    (*(object))[member.first] = value;        
                } break;

                case 'f':
                case 't': {
                    value = parseBoolean(member.second);
                    (*(object))[member.first] = value; 
                } break;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '-': {
                    int pointPos = member.second.find_first_of(".eE");
                    
                    if (pointPos == std::string::npos) {
                        value = parseInteger(member.second);
                    } else {
                        value = parseFloatingPoint(member.second);
                    }

                    (*(object))[member.first] = value; 
                } break;

                case '"': {
                    value = parseString(member.second);
                    (*(object))[member.first] = value; 
                } break;

                case '[': {
                    value = parseArray(member.second);
                    (*(object))[member.first] = value; 
                } break;

                case '{': {
                    value = parseObject(member.second);
                    (*(object))[member.first] = value; 
                } break;
            }
        }

        json->type = Type::Object;
        json->value.object = object;

        return json;
    }

    Json *Json::fromCppString(const std::string &input) {
        Json *json = new Json();
        int charFound = 0;
        bool object = false;
        bool array = false;

        for (const auto &c : input) {
            if (c == '{') {
                if (charFound == 0) {
                    object = true;
                    break;
                }
            } else if (c == '[') {
                if (charFound == 0) {
                    array = true;
                    break;
                }
            } else if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
                // do nothing
            } else {
                charFound++;
            }
        }

        if (!object && !array) {
            return json;
        } else if (object) {
            delete json;
            json = parseObject(input);
        } else if (array) {
            delete json;
            json = parseArray(input);
        }

        return json;
    }

    bool Json::operator==(nullptr_t null) const {
        if (type == Type::Null) {
            return true;
        }
        
        return false;
    }

    void Json::operator=(nullptr_t null) {
        type = Type::Null;
    }

    
    bool Json::operator==(bool boolean) const {
        if (type != Type::Boolean) {
            throw WrongTypeException();
        }

        return value.boolean == boolean;
    }

    void Json::operator=(bool boolean) {
        type = Type::Boolean;
        value.boolean = boolean;
    }

    bool Json::operator==(int integer) const {
        if (type != Type::Integer) {
            throw WrongTypeException();
        }

        return value.integer == (long long)integer;
    }

    void Json::operator=(int integer) {
        type = Type::Integer;
        value.integer = (long long)integer;
    }

    bool Json::operator==(long integer) const {
        if (type != Type::Integer) {
            throw WrongTypeException();
        }

        return value.integer == (long long)integer;
    }

    void Json::operator=(long integer) {
        type = Type::Integer;
        value.integer = (long long)integer;
    }

    bool Json::operator==(long long integer) const {
        if (type != Type::Integer) {
            throw WrongTypeException();
        }

        return value.integer == integer;
    }

    void Json::operator=(long long integer) {
        type = Type::Integer;
        value.integer = integer;
    }

    bool Json::operator==(float floatingPoint) const {
        if (type != Type::FloatingPoint) {
            throw WrongTypeException();
        }

        return (value.floatingPoint >= (long double)floatingPoint - 0.01) &&
                (value.floatingPoint <= (long double)floatingPoint + 0.01);
    }

    void Json::operator=(float floatingPoint) {
        type = Type::FloatingPoint;
        value.floatingPoint = (long double)floatingPoint;
    }

    bool Json::operator==(double floatingPoint) const {
        if (type != Type::FloatingPoint) {
            throw WrongTypeException();
        }

        return (value.floatingPoint >= (long double)floatingPoint - 0.01) &&
                (value.floatingPoint <= (long double)floatingPoint + 0.01);
    }

    void Json::operator=(double floatingPoint) {
        type = Type::FloatingPoint;
        value.floatingPoint = (long double)floatingPoint;
    }
    
    bool Json::operator==(long double floatingPoint) const {
        if (type != Type::FloatingPoint) {
            throw WrongTypeException();
        }

        return (value.floatingPoint >= floatingPoint - 0.01) &&
                (value.floatingPoint <= floatingPoint + 0.01);
    }

    void Json::operator=(long double floatingPoint) {
        type = Type::FloatingPoint;
        value.floatingPoint = floatingPoint;
    }



    bool Json::operator==(std::string string) const {
        if (type != Type::String) {
            throw WrongTypeException();
        }

        return *(value.string) == string;
    }

    void Json::operator=(const std::string &string) {
        type = Type::String;
        value.string = new std::string(string);
    }

    bool Json::operator==(const char *string) const {
        if (type == Type::String) {
            if (*value.string == std::string(string)) {
                return true;
            }
        }

        return false;
    }

    void Json::operator=(const char *string) {
        type = Type::String;
        value.string = new std::string(string);
    }


    bool Json::operator==(Json other) const {
        if (type == other.type) {
            switch(type) {
                case Type::Boolean: return value.boolean == other.value.boolean; break;
                case Type::Integer: return value.integer == other.value.integer; break;
                case Type::FloatingPoint: return value.floatingPoint == other.value.floatingPoint; break;
                case Type::String: return *(value.string) == *(other.value.string); break;
                case Type::Array: return *(value.array) == *(other.value.array); break;
                case Type::Object: return *(value.object) == *(other.value.object); break;
                default: return true; break;
            }
        } 
        
        return false;
    }

    void Json::operator=(const Json &other) {
        type = other.type;

        switch (type) {
            case Type::Boolean: {
                value.boolean = other.value.boolean;
            } break;

            case Type::Integer: {
                value.integer = other.value.integer;
            } break;

            case Type::FloatingPoint: {
                value.floatingPoint = other.value.floatingPoint;
            } break;

            case Type::String: {
                const auto otherString = *other.value.string;
                value.string = new std::string(otherString);
            } break;

            case Type::Array: {
                const auto otherArray = *other.value.array;
                value.array = new std::vector<Json *>(otherArray);
            } break;

            case Type::Object: {
                const auto otherObject = *other.value.object;
                value.object = new std::unordered_map<std::string, Json *>(otherObject);
            } break;
        }
    }

    void Json::operator=(const Json *other) {
        type = other->type;

        switch (type) {
            case Type::Boolean: {
                value.boolean = other->value.boolean;
            } break;

            case Type::Integer: {
                value.integer = other->value.integer;
            } break;

            case Type::FloatingPoint: {
                value.floatingPoint = other->value.floatingPoint;
            } break;

            case Type::String: {
                const auto otherString = *other->value.string;
                value.string = new std::string(otherString);
            } break;

            case Type::Array: {
                const auto otherArray = *other->value.array;
                value.array = new std::vector<Json *>(otherArray);
            } break;

            case Type::Object: {
                const auto otherObject = *other->value.object;
                value.object = new std::unordered_map<std::string, Json *>(otherObject);
            } break;
        }
    }


    Json Json::operator[](int index) const {
        if (type != Type::Array) {
            throw WrongTypeException();
        }

        return *(value.array->at(index));
    }

    Json Json::operator[](const char *key) const {
        if (type != Type::Object) {
            throw WrongTypeException();
        }

        return *(value.object->at(std::string(key)));
    }


    Json::Type Json::getType() const {
        return type;
    }

    Json::operator bool() {
        return value.boolean;
    }

    Json::operator int() {
        return (int)value.integer;
    }

    Json::operator long() {
        return (long)value.integer;
    }

    Json::operator long long() {
        return (long long)value.integer;
    }

    Json::operator float() {
        return (float)value.floatingPoint;
    }

    Json::operator double() {
        return (double)value.floatingPoint;
    }

    Json::operator long double() {
        return (long double)value.floatingPoint;
    }

    Json::operator std::string() {
        return *value.string;
    }

    std::ostream &operator<<(std::ostream &output, const Json &json) {
        switch (json.type) {
            case Json::Type::Invalid: output << "invalid data"; break; 
            case Json::Type::Null: output << "null"; break;
            case Json::Type::Boolean:
                if (json == true) {
                    output << "true";
                } else {
                    output << "false";
                }
                break;
            case Json::Type::Integer: output << json.value.integer; break;
            case Json::Type::FloatingPoint: output << json.value.floatingPoint; break;
            case Json::Type::String: output << *json.value.string; break;
            case Json::Type::Array:
                output << "[ ";
                for (const auto &element : *json.value.array) {
                    output << *element << ", ";
                }
                output << "]";
                break;
            case Json::Type::Object:
                output << "\n{\n";
                for (const auto &pair: *json.value.object) {
                    output << '\t' << pair.first << ": " << *pair.second << ',' << std::endl;
                }
                output << "}\n";
                break;
        }

        return output;
    }

}; // namespace JSON