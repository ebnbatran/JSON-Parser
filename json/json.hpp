#include <string>
#include <vector>
#include <unordered_map>
#include <exception>
#include <ostream>
#include <variant>

#include "utility.hpp"

namespace JSON {

    class WrongTypeException : public std::exception {
        public:
            ~WrongTypeException() override {};
            const char *what() const noexcept override;
    };


    class Json {
        
        friend std::ostream &operator<<(std::ostream &output, const Json &json);

        using JsonString = std::string *;
        using JsonArray = std::vector<Json *> *;
        using JsonObject = std::unordered_map<std::string, Json *> *;

        public:
            enum Type {
                Boolean,
                Integer,
                FloatingPoint,
                String,
                Array,
                Object,
                Null,
                Invalid
            };  

        public:
            Json();
            Json(const Type &type);
            Json(const Json *other);
            ~Json();

            /**
             * This method returns a pointer to a Json value
             * extracted from the given std::string object
             * 
             * @param[in] input
             *     The string object to be parsed.
             * 
             * @return
             *     A pointer to the Json value which was parsed
             * */
            static Json *fromCppString(const std::string &input);

        private:
            Type type;

            std::variant< 
                bool, 
                long long, 
                long double, 
                JsonString, 
                JsonArray, 
                JsonObject
            > value;
 
        public:
            static Json *parseBoolean(const std::string &input);
            static Json *parseNull(const std::string &input);
            static Json *parseInteger(const std::string &input);
            static Json *parseFloatingPoint(const std::string &input);
            static Json *parseString(const std::string &input);
            static Json *parseArray(const std::string &input);
            static Json *parseObject(const std::string &input);
        
        public:
            bool operator==(nullptr_t null) const;
            void operator=(nullptr_t null);
            
            bool operator==(bool boolean) const;
            void operator=(bool boolean);

            bool operator==(int integer) const;
            void operator=(int integer);
            bool operator==(long integer) const;
            void operator=(long integer);
            bool operator==(long long integer) const;
            void operator=(long long integer);

            bool operator==(float floatingPoint) const;
            void operator=(float floatingPoint);
            bool operator==(double floatingPoint) const;
            void operator=(double floatingPoint);
            bool operator==(long double floatingPoint) const;
            void operator=(long double floatingPoint);

            bool operator==(std::string string) const;
            void operator=(const std::string &string);
            bool operator==(const char *string) const;
            void operator=(const char *string);

            bool operator==(const Json &other) const;
            void operator=(const Json &other);
            void operator=(const Json *other);

            Json operator[](int index) const;
            Json operator[](const char *key) const;
        
        public:
            Type getType() const;

            bool isInvalid() const { return type == Type::Invalid; }
            bool isNull() const { return type == Type::Null; }
            bool isBoolean() const { return type == Type::Boolean; }
            bool isInteger() const { return type == Type::Integer; }
            bool isFloatingPoint() const { return type == Type::FloatingPoint; }
            bool isString() const { return type == Type::String; }
            bool isArray() const { return type == Type::Array; }
            bool isObject() const { return type == Type::Object; }

            operator bool();
            operator int();
            operator long();
            operator long long();
            operator float();
            operator double();
            operator long double();
            operator std::string();
    };

    std::ostream &operator<<(std::ostream &output, const Json &json);

}; // namespace JSON
