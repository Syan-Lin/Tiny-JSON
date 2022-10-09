#include <iostream>
#include "../tiny_json.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include "ThirdParty/doctest.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

string test_string(string str){
    auto json = jp(str).parse_number();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_Number"){
    detail = false;
    SUBCASE("Json"){
        JSON5 = false;

        CHECK(test_string("1.1, ") == "1.100000");
        CHECK(test_string("123,") == "123");
        CHECK(test_string("123.1] ") == "123.100000");
        CHECK(test_string("123]") == "123");
        CHECK(test_string("123.22} ") == "123.220000");
        CHECK(test_string("123}") == "123");
        CHECK(test_string("123") == "123");
        CHECK(test_string("123.22") == "123.220000");

        SUBCASE("Json_Error"){
            CHECK(test_string("error") == "null");
            CHECK(test_string("1.1.1") == "1.100000");
            CHECK(test_string("0xb3") == "null");
            CHECK(test_string("b123") == "null");
        }
    }
    SUBCASE("Json5"){
        JSON5 = true;

        CHECK(test_string("123.22, ") == "123.220000");
        CHECK(test_string("123,") == "123");
        CHECK(test_string("123.22] ") == "123.220000");
        CHECK(test_string("123]") == "123");
        CHECK(test_string("123.22} ") == "123.220000");
        CHECK(test_string("123}") == "123");
        CHECK(test_string("123") == "123");
        CHECK(test_string("123.22") == "123.220000");
        CHECK(test_string("0xb3") == "0xb3");

        SUBCASE("Json5_Error"){
            CHECK(test_string("error") == "null");
            CHECK(test_string("1.1.1") == "1.100000");
            CHECK(test_string("0xb3.123") == "0xb3");
            CHECK(test_string("b123") == "null");
        }
    }
}
