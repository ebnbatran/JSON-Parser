#include <gtest/gtest.h>

#include "../json/json.hpp"

namespace JSON {
    
    TEST(JSONTestSuite, testParseNull) {
        const auto jsonNull = Json::parseNull("null");
        const auto jsonNullInvalid = Json::parseNull("Null");

        ASSERT_TRUE(jsonNull->isNull());
        ASSERT_TRUE(jsonNullInvalid->isInvalid());
    }

    TEST(JSONTestSuite, testParseBoolean) {
        const auto jsonTrue = Json::parseBoolean("true");
        const auto jsonFalse = Json::parseBoolean("false");
        
        ASSERT_TRUE(jsonTrue->isBoolean());
        ASSERT_EQ(*jsonTrue, true);

        ASSERT_TRUE(jsonFalse->isBoolean());
        ASSERT_EQ(*jsonFalse, false);
    }

    TEST(JSONTestSuite, testParseInteger) {
        const auto jsonPositive = Json::parseInteger("1234");
        const auto jsonNegative = Json::parseInteger("-8736");
        const auto jsonZeroes = Json::parseInteger("50000");

        ASSERT_TRUE(jsonPositive->isInteger());
        ASSERT_EQ(*jsonPositive, 1234);

        ASSERT_TRUE(jsonNegative->isInteger());
        ASSERT_EQ(*jsonNegative, -8736);

        ASSERT_TRUE(jsonZeroes->isInteger());
        ASSERT_EQ(*jsonZeroes, 50000);
    }

    TEST(JSONTestSuite, testParseFloatingPoint) {
        const auto jsonPositive = Json::parseFloatingPoint("12.34");
        const auto jsonNegative = Json::parseFloatingPoint("-87.36");
        const auto jsonZeroes = Json::parseFloatingPoint("500.00");
        const auto jsonPowerOnPositive = Json::parseFloatingPoint("345.23e5");
        const auto jsonPowerOnNegative = Json::parseFloatingPoint("-234.012e8");
        const auto jsonNegativePower = Json::parseFloatingPoint("38e-3");

        ASSERT_TRUE(jsonPositive->isFloatingPoint());
        ASSERT_DOUBLE_EQ(*jsonPositive, 12.34);

        ASSERT_TRUE(jsonNegative->isFloatingPoint());
        ASSERT_DOUBLE_EQ(*jsonNegative, -87.36);

        ASSERT_TRUE(jsonZeroes->isFloatingPoint());
        ASSERT_DOUBLE_EQ(*jsonZeroes, 500.00);

        ASSERT_TRUE(jsonPowerOnPositive->isFloatingPoint());
        ASSERT_DOUBLE_EQ(*jsonPowerOnPositive, std::pow(345.23, 5));

        ASSERT_TRUE(jsonPowerOnNegative->isFloatingPoint());
        ASSERT_DOUBLE_EQ(*jsonPowerOnNegative, std::pow(-234.012, 8));

        ASSERT_TRUE(jsonNegativePower->isFloatingPoint());
        ASSERT_DOUBLE_EQ(*jsonNegativePower, std::pow(38,-3));
    }

    TEST(JSONTestSuite, testParseStringValue) {
        std::string greeting = "Hi!, My name is ebrahim!";
        const auto jsonString = Json::parseString("\"Hi!, My name is ebrahim!\"");

        ASSERT_TRUE(jsonString->isString());
        ASSERT_EQ(*jsonString, greeting);
        ASSERT_EQ(*jsonString, "Hi!, My name is ebrahim!");
    }

    TEST(JSONTestSuite, testParseArray) {
        const auto jsonArray = Json::parseArray("[\n\t\"Ebrahim Ahmad\",\n\t-15.23e5,\n\t[\n\t\t1,\n\t\t2\n\t]\n]");

        ASSERT_TRUE(jsonArray->isArray());
        ASSERT_EQ((*jsonArray)[0], "Ebrahim Ahmad");
        ASSERT_DOUBLE_EQ((*jsonArray)[1], std::pow(-15.23, 5));
        ASSERT_EQ((*jsonArray)[2][0], 1);
        ASSERT_EQ((*jsonArray)[2][1], 2);
    }

    TEST(JSONTestSuite, testParseObjectValue) {
        const auto jsonObject = Json::parseObject("{\"name\": \"ebrahim ahmad batran\",\"age\": 27,\"job\": \"student\",\"salary\": null,\"favorite_numbers\": [12,14,30],\"siblings\": [\"norhan\",\"mahmoud\",\"batran\"],\"parents\": [{\"name\": \"ahmad\",\"age\": 64,\"job\": \"teacher\"},{\"name\": \"tahany\",\"age\": 59,\"job\": \"doctor\"}] }");

        ASSERT_TRUE(jsonObject->isObject());
        ASSERT_EQ((*jsonObject)["name"], "ebrahim ahmad batran");
        ASSERT_EQ((*jsonObject)["age"], 27);
        ASSERT_EQ((*jsonObject)["job"], "student");
        ASSERT_TRUE((*jsonObject)["salary"].isNull());
        ASSERT_EQ((*jsonObject)["favorite_numbers"][0], 12);
        ASSERT_EQ((*jsonObject)["parents"][0]["age"], 64);
    }

    TEST(JSONTestSuite, testParseCppString) {
        const auto jsonArray = Json::fromCppString("[{\"name\": \"ebrahim\", \"age\": 27}, {\"name\": \"norhan\", \"age\": 24}]");
        const auto jsonObject = Json::fromCppString("{\"name\": \"mahmoud\", \"age\": 21, \"plays_sports\": true, \"numbers\": [1, 2.34, 4]}");

        ASSERT_EQ((*jsonArray)[0]["age"], 27);
        ASSERT_EQ((*jsonArray)[1]["name"], "norhan");

        ASSERT_EQ((*jsonObject)["age"], 21);
        ASSERT_DOUBLE_EQ((*jsonObject)["numbers"][1], 2.34);
    }

    TEST(JSONTestSuite, testOperators) {
        Json jsonArray = Json::parseArray("[1, 2.5, 3]");
        Json jsonObject = Json::parseObject("{\"name\": \"ebrahim\"}");
        Json jsonString = Json::parseString("\"Hello\"");
        Json jsonBoolean = Json::parseBoolean("true");
        Json jsonNull = Json::parseNull("null");
        Json jsonInvalid = Json::parseNull("Null");
        Json json;
        json = jsonArray;
        Json jsonLargeObject = Json::fromCppString("{\"numbers\": [1, {\"name\": \"ebrahim\"}, 3]}");

        ASSERT_TRUE(jsonArray.isArray());
        ASSERT_EQ(jsonArray[0], 1);

        ASSERT_TRUE(jsonObject.isObject());
        ASSERT_EQ(jsonObject["name"], "ebrahim");

        ASSERT_TRUE(jsonString.isString());
        ASSERT_EQ(jsonString, "Hello");

        ASSERT_TRUE(jsonBoolean.isBoolean());
        ASSERT_EQ(jsonBoolean, true);

        ASSERT_TRUE(jsonNull.isNull());
        ASSERT_EQ(jsonNull, nullptr);

        ASSERT_TRUE(jsonInvalid.isInvalid());

        ASSERT_TRUE(json.isArray());
        ASSERT_EQ(json[1], 2.5);

        ASSERT_TRUE(jsonLargeObject.isObject());
        ASSERT_EQ(jsonLargeObject["numbers"][1]["name"], "ebrahim");
    }
};