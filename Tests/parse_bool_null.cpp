#include <iostream>
#include "../tiny_json.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include "ThirdParty/doctest.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

string test_true(string str){
    auto json = jp(str).parse_true();
    return json.parse();
}
string test_false(string str){
    auto json = jp(str).parse_false();
    return json.parse();
}
string test_null(string str){
    auto json = jp(str).parse_null();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_Fixed"){
    detail = false;

    CHECK(test_true("true") == "true");
    CHECK(test_false("false") == "false");
    CHECK(test_null("null") == "null");
    CHECK(test_true("true,") == "true");
    CHECK(test_false("false,") == "false");
    CHECK(test_null("null,") == "null");

    SUBCASE("Error"){
        CHECK(test_true("ture") == "null");
        CHECK(test_false("flase") == "null");
        CHECK(test_null("nuul") == "null");
        CHECK(test_true("tru") == "null");
        CHECK(test_false("fals") == "null");
        CHECK(test_null("nul") == "null");
    }
}
