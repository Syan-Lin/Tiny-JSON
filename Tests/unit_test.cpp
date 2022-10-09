#include <iostream>
#include "../tiny_json.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include "ThirdParty/doctest.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

string test_annotation(string str){
    string after;
    auto parser = jp(str);
    while(parser.index < str.size()){
        parser.skip_annotation();
        if(parser.index >= str.size()) break;
        after += str[parser.index++];
    }
    return after;
}
string test_blank(string str){
    string after;
    auto parser = jp(str);
    while(parser.index < str.size()){
        parser.skip_blanks();
        if(parser.index >= str.size()) break;
        after += str[parser.index++];
    }
    return after;
}

// Corverage: 100%
TEST_CASE("Read_Test"){
    detail = false;
    SUBCASE("Annotation"){
        CHECK(test_annotation("info//anno\ninfo") == "infoinfo");
        CHECK(test_annotation("//anno\ninfo") == "info");
        CHECK(test_annotation("info//anno\n") == "info");
        CHECK(test_annotation("info//anno") == "info");
        CHECK(test_annotation("info//") == "info");
        CHECK(test_annotation("//info") == "");
        CHECK(test_annotation("/*anno*/info") == "info");
        CHECK(test_annotation("info/*anno*/") == "info");
        CHECK(test_annotation("/**/info") == "info");
        CHECK(test_annotation("info/**/") == "info");
        CHECK(test_annotation("info/**/info") == "infoinfo");
        CHECK(test_annotation("info/**/info") == "infoinfo");
        CHECK(test_annotation("/*anno\n\t*/info") == "info");
        CHECK(test_annotation("info/*anno\n\t*/") == "info");
        CHECK(test_annotation("info/*anno\n\t*/info") == "infoinfo");
        CHECK(test_annotation("info/*anno\n\t*/info") == "infoinfo");

        SUBCASE("Error"){
            CHECK(test_annotation("info/_error") == "info_error");
            CHECK(test_annotation("info/") == "info");
            CHECK(test_annotation("error/*") == "error");
            CHECK(test_annotation("error/**") == "error");
            CHECK(test_annotation("/*error") == "r");
            CHECK(test_annotation("error/*error") == "errorr");
        }
    }

    SUBCASE("Blank"){
        CHECK(test_blank("information  \n    ") == "information");
        CHECK(test_blank("  \n    information") == "information");
        CHECK(test_blank("  \ninformation    ") == "information");
        CHECK(test_blank("in  form  a\n \n  t    io      n") == "information");
        CHECK(test_blank(" in  form  a\n \n  t    io      n ") == "information");
    }
}

string test_arr(string str){
    auto json = jp(str).parse_array();
    return json.parse();
}
string test_obj(string str){
    auto json = jp(str).parse_object();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_Array_Object"){
    SUBCASE("Json"){
        JSON5 = false;

        SUBCASE("Array"){
            CHECK(test_arr(R"([1,"hello",true,false,null])") == R"([1,"hello",true,false,null])");
            CHECK(test_arr(R"([ 1, "hello" ,true, false ,null ])") == R"([1,"hello",true,false,null])");
            CHECK(test_arr("[\t 1, \n\"hello\" ,\ntrue\t, \nfalse \t,null \n\t]") == R"([1,"hello",true,false,null])");
            CHECK(test_arr(R"([[1,1],["a","a"],[true,false],[null,null]])") == R"([[1,1],["a","a"],[true,false],[null,null]])");
            CHECK(test_arr(R"([[1 , 1],[" a ", "[a]"] , [ true, false], [ null,null] ])") == R"([[1,1],[" a ","[a]"],[true,false],[null,null]])");
            CHECK(test_arr(R"([{"str:":"abc"},[1, 3]])") == R"([{"str:":"abc"},[1,3]])");
            CHECK(test_arr(R"([[1,1,],["a","a",],[true,false,],[null,null,],])") == R"([[1,1],["a","a"],[true,false],[null,null]])");
            CHECK(test_arr(R"([[1,1 , ],["a","a" , ], [true,  false ,],[null,null,  ]  ,])") == R"([[1,1],["a","a"],[true,false],[null,null]])");
        }
        SUBCASE("Object"){
            CHECK(test_obj(R"({"str":"hello","number":1})") == R"({"number":1,"str":"hello"})");
            CHECK(test_obj(R"({ "str" : "hello" ,"number":1 })") == R"({"number":1,"str":"hello"})");
            CHECK(test_obj(R"({"obj":{"name":"jane","arr":[1,2,3]},"number":1,"weight":null})") == R"({"number":1,"obj":{"arr":[1,2,3],"name":"jane"},"weight":null})");
            CHECK(test_obj(R"({"obj":{"name":"jane","arr":[1,2,3],},"number":1,"weight":null,})") == R"({"number":1,"obj":{"arr":[1,2,3],"name":"jane"},"weight":null})");
        }

        SUBCASE("Json_Error"){
            SUBCASE("Array"){
                CHECK(test_arr(R"([1,"hello" a,true,false,null])") == R"(null)");
                CHECK(test_arr(R"([1,"hello"true,false,null])") == R"(null)");
                CHECK(test_arr(R"([1,"hello",key,false,null])") == R"(null)");
            }
            SUBCASE("Object"){
                CHECK(test_obj(R"(str:"hello","number":1})") == R"(null)");
                CHECK(test_obj(R"({str:"hello","number":1})") == R"(null)");
                CHECK(test_obj(R"({str:"hello","number":1,,})") == R"(null)");
                CHECK(test_obj(R"({"str:"hello","number":1})") == R"(null)");
                CHECK(test_obj(R"({str":"hello","number":1})") == R"(null)");
                CHECK(test_obj(R"({str","hello","number":1})") == R"(null)");
                CHECK(test_obj(R"({str","hello","number":1})") == R"(null)");
                CHECK(test_obj(R"({'str',"hello","number":1)") == R"(null)");
            }
        }
    }
    SUBCASE("JSON5"){
        JSON5 = true;
        CHECK(test_obj(R"({str:'hello',number:1,})") == R"({number:1,str:'hello'})");
    }
}

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

string test_number(string str){
    auto json = jp(str).parse_number();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_Number"){
    SUBCASE("Json"){
        JSON5 = false;

        CHECK(test_number("1.1, ") == "1.100000");
        CHECK(test_number("123,") == "123");
        CHECK(test_number("123.1] ") == "123.100000");
        CHECK(test_number("123]") == "123");
        CHECK(test_number("123.22} ") == "123.220000");
        CHECK(test_number("123}") == "123");
        CHECK(test_number("123") == "123");
        CHECK(test_number("123.22") == "123.220000");

        SUBCASE("Json_Error"){
            CHECK(test_number("error") == "null");
            CHECK(test_number("1.1.1") == "1.100000");
            CHECK(test_number("0xb3") == "null");
            CHECK(test_number("b123") == "null");
        }
    }
    SUBCASE("Json5"){
        JSON5 = true;

        CHECK(test_number("123.22, ") == "123.220000");
        CHECK(test_number("123,") == "123");
        CHECK(test_number("123.22] ") == "123.220000");
        CHECK(test_number("123]") == "123");
        CHECK(test_number("123.22} ") == "123.220000");
        CHECK(test_number("123}") == "123");
        CHECK(test_number("123") == "123");
        CHECK(test_number("123.22") == "123.220000");
        CHECK(test_number("0xb3") == "0xb3");

        SUBCASE("Json5_Error"){
            CHECK(test_number("error") == "null");
            CHECK(test_number("1.1.1") == "1.100000");
            CHECK(test_number("0xb3.123") == "0xb3");
            CHECK(test_number("b123") == "null");
        }
    }
}

string test_string(string str){
    auto json = jp(str).parse_string();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_String"){
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