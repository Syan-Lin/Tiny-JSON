#include <iostream>
#include "../tiny_json.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include "ThirdParty/doctest.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

string test_string(string str){
    auto json = jp(str).parse_string();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_String"){
    detail = false;
    SUBCASE("Json"){
        JSON5 = false;
        CHECK(test_string(R"("'hello'\t\n")") == R"("'hello'\t\n")");
        CHECK(test_string(R"("\"hello\"\t\n")") == R"("\"hello\"\t\n")");
        CHECK(test_string(R"("a message")") == R"("a message")");
        CHECK(test_string(R"("\u0022\u4f60\u597d\u002c\u0043\u002b\u002b\u0022")") == R"("\"你好,C++\"")");
        CHECK(test_string(R"("\u0027\u4f60\u597d\u002c\u0043\u002b\u002b\u0027")") == R"("'你好,C++'")");
        CHECK(test_string(R"("")") == R"("")");
        CHECK(test_string(R"("\"\b\f\t\n\r\\")") == R"("\"\b\f\t\n\r\\")");

        SUBCASE("Json_Error"){
            CHECK(test_string(R"("hello)") == R"(null)");
            CHECK(test_string(R"("hello')") == R"(null)");
            CHECK(test_string(R"('hello')") == R"(null)");
            CHECK(test_string(R"('hello")") == R"(null)");
            CHECK(test_string("\"hello\n\"") == R"(null)");
            CHECK(test_string(R"("hello\'")") == R"(null)");
            CHECK(test_string(R"("hell"o")") == R"("hell")");
            CHECK(test_string(R"("hello\u002")") == R"(null)");
            CHECK(test_string(R"("\hello")") == R"(null)");
            CHECK(test_string(R"("hello\)") == R"(null)");
        }
    }
    SUBCASE("Json5"){
        JSON5 = true;
        CHECK(test_string(R"("'hello'\t\n")") == R"('\'hello\'\t\n')");
        CHECK(test_string(R"("\"hello\"\t\n")") == R"('"hello"\t\n')");
        CHECK(test_string(R"("a message")") == R"('a message')");
        CHECK(test_string(R"("\u0022\u4f60\u597d\u002c\u0043\u002b\u002b\u0022")") == R"('"你好,C++"')");
        CHECK(test_string(R"("\u0027\u4f60\u597d\u002c\u0043\u002b\u002b\u0027")") == R"('\'你好,C++\'')");
        CHECK(test_string(R"("")") == R"('')");
        CHECK(test_string(R"("\"\b\f\t\n\r\\")") == R"('"\b\f\t\n\r\\')");
        CHECK(test_string("\"hell\n\t  o\"") == R"('hello')");

        CHECK(test_string(R"('\'hello\'\t\n')") == R"('\'hello\'\t\n')");
        CHECK(test_string(R"('"hello"\t\n')") == R"('"hello"\t\n')");
        CHECK(test_string(R"('a message')") == R"('a message')");
        CHECK(test_string(R"('\u0022\u4f60\u597d\u002c\u0043\u002b\u002b\u0022')") == R"('"你好,C++"')");
        CHECK(test_string(R"('\u0027\u4f60\u597d\u002c\u0043\u002b\u002b\u0027')") == R"('\'你好,C++\'')");
        CHECK(test_string(R"('')") == R"('')");
        CHECK(test_string(R"('"\b\f\t\n\r\\')") == R"('"\b\f\t\n\r\\')");
        CHECK(test_string("'hell\n\t  o'") == R"('hello')");

        SUBCASE("Json5_Error"){
            CHECK(test_string(R"("hello)") == R"(null)");
            CHECK(test_string(R"("hello')") == R"(null)");
            CHECK(test_string(R"('hello")") == R"(null)");
            CHECK(test_string(R"("hello\'")") == R"(null)");
            CHECK(test_string(R"("hell"o")") == R"('hell')");
            CHECK(test_string(R"('hell'o')") == R"('hell')");
            CHECK(test_string(R"("hello\u002")") == R"(null)");
            CHECK(test_string(R"("\hello")") == R"(null)");
            CHECK(test_string(R"("hello\)") == R"(null)");
            CHECK(test_string(R"('\"hello')") == R"(null)");
        }
    }
}