#include <iostream>
#include "../tiny_json.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include "ThirdParty/doctest.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

string testAnnotation(string str){
    string after;
    auto parser = jp(str);
    while(parser.index < str.size()){
        parser.skipAnnotation();
        if(parser.index >= str.size()) break;
        after += str[parser.index++];
    }
    return after;
}
string testBlank(string str){
    string after;
    auto parser = jp(str);
    while(parser.index < str.size()){
        parser.skipBlanks();
        if(parser.index >= str.size()) break;
        after += str[parser.index++];
    }
    return after;
}

// Corverage: 100%
TEST_CASE("Read_Test"){
    detail = false;
    SUBCASE("Annotation"){
        CHECK(testAnnotation("info//anno\ninfo") == "infoinfo");
        CHECK(testAnnotation("//anno\ninfo") == "info");
        CHECK(testAnnotation("info//anno\n") == "info");
        CHECK(testAnnotation("info//anno") == "info");
        CHECK(testAnnotation("info//") == "info");
        CHECK(testAnnotation("//info") == "");
        CHECK(testAnnotation("/*anno*/info") == "info");
        CHECK(testAnnotation("info/*anno*/") == "info");
        CHECK(testAnnotation("/**/info") == "info");
        CHECK(testAnnotation("info/**/") == "info");
        CHECK(testAnnotation("info/**/info") == "infoinfo");
        CHECK(testAnnotation("info/**/info") == "infoinfo");
        CHECK(testAnnotation("/*anno\n\t*/info") == "info");
        CHECK(testAnnotation("info/*anno\n\t*/") == "info");
        CHECK(testAnnotation("info/*anno\n\t*/info") == "infoinfo");
        CHECK(testAnnotation("info/*anno\n\t*/info") == "infoinfo");

        SUBCASE("Error"){
            CHECK(testAnnotation("info/_error") == "info_error");
            CHECK(testAnnotation("info/") == "info");
            CHECK(testAnnotation("error/*") == "error");
            CHECK(testAnnotation("error/**") == "error*");
            CHECK(testAnnotation("/*error") == "r");
            CHECK(testAnnotation("error/*error") == "errorr");
        }
    }

    SUBCASE("Blank"){
        CHECK(testBlank("information  \n    ") == "information");
        CHECK(testBlank("  \n    information") == "information");
        CHECK(testBlank("  \ninformation    ") == "information");
        CHECK(testBlank("in  form  a\n \n  t    io      n") == "information");
        CHECK(testBlank(" in  form  a\n \n  t    io      n ") == "information");
    }
}

string testArr(string str){
    auto json = jp(str).parseArray();
    return json.parse();
}
string testObj(string str){
    auto json = jp(str).parseObject();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_Array_Object"){
    SUBCASE("Json"){
        JSON5 = false;

        SUBCASE("Array"){
            CHECK(testArr(R"([1,"hello",true,false,null])") == R"([1,"hello",true,false,null])");
            CHECK(testArr(R"([ 1, "hello" ,true, false ,null ])") == R"([1,"hello",true,false,null])");
            CHECK(testArr("[\t 1, \n\"hello\" ,\ntrue\t, \nfalse \t,null \n\t]") == R"([1,"hello",true,false,null])");
            CHECK(testArr(R"([[1,1],["a","a"],[true,false],[null,null]])") == R"([[1,1],["a","a"],[true,false],[null,null]])");
            CHECK(testArr(R"([[1 , 1],[" a ", "[a]"] , [ true, false], [ null,null] ])") == R"([[1,1],[" a ","[a]"],[true,false],[null,null]])");
            CHECK(testArr(R"([{"str:":"abc"},[1, 3]])") == R"([{"str:":"abc"},[1,3]])");
            CHECK(testArr(R"([[1,1,],["a","a",],[true,false,],[null,null,],])") == R"([[1,1],["a","a"],[true,false],[null,null]])");
            CHECK(testArr(R"([[1,1 , ],["a","a" , ], [true,  false ,],[null,null,  ]  ,])") == R"([[1,1],["a","a"],[true,false],[null,null]])");
        }
        SUBCASE("Object"){
            CHECK(testObj(R"({"str":"hello","number":1})") == R"({"number":1,"str":"hello"})");
            CHECK(testObj(R"({ "str" : "hello" ,"number":1 })") == R"({"number":1,"str":"hello"})");
            CHECK(testObj(R"({"obj":{"name":"jane","arr":[1,2,3]},"number":1,"weight":null})") == R"({"number":1,"obj":{"arr":[1,2,3],"name":"jane"},"weight":null})");
            CHECK(testObj(R"({"obj":{"name":"jane","arr":[1,2,3],},"number":1,"weight":null,})") == R"({"number":1,"obj":{"arr":[1,2,3],"name":"jane"},"weight":null})");
        }

        SUBCASE("Json_Error"){
            SUBCASE("Array"){
                CHECK(testArr(R"([1,"hello" a,true,false,null])") == R"(null)");
                CHECK(testArr(R"([1,"hello"true,false,null])") == R"(null)");
                CHECK(testArr(R"([1,"hello",key,false,null])") == R"(null)");
            }
            SUBCASE("Object"){
                CHECK(testObj(R"(str:"hello","number":1})") == R"(null)");
                CHECK(testObj(R"({str:"hello","number":1})") == R"(null)");
                CHECK(testObj(R"({str:"hello","number":1,,})") == R"(null)");
                CHECK(testObj(R"({"str:"hello","number":1})") == R"(null)");
                CHECK(testObj(R"({str":"hello","number":1})") == R"(null)");
                CHECK(testObj(R"({str","hello","number":1})") == R"(null)");
                CHECK(testObj(R"({str","hello","number":1})") == R"(null)");
                CHECK(testObj(R"({'str',"hello","number":1)") == R"(null)");
            }
        }
    }
    SUBCASE("JSON5"){
        JSON5 = true;
        CHECK(testObj(R"({str:'hello',number:1,})") == R"({number:1,str:'hello'})");
    }
}

string testTrue(string str){
    auto json = jp(str).parseTrue();
    return json.parse();
}
string testFalse(string str){
    auto json = jp(str).parseFalse();
    return json.parse();
}
string testNull(string str){
    auto json = jp(str).parseNull();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_Fixed"){
    CHECK(testTrue("true") == "true");
    CHECK(testFalse("false") == "false");
    CHECK(testNull("null") == "null");
    CHECK(testTrue("true,") == "true");
    CHECK(testFalse("false,") == "false");
    CHECK(testNull("null,") == "null");

    SUBCASE("Error"){
        CHECK(testTrue("ture") == "null");
        CHECK(testFalse("flase") == "null");
        CHECK(testNull("nuul") == "null");
        CHECK(testTrue("tru") == "null");
        CHECK(testFalse("fals") == "null");
        CHECK(testNull("nul") == "null");
    }
}

string testNumber(string str){
    auto json = jp(str).parseNumber();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_Number"){
    SUBCASE("Json"){
        JSON5 = false;

        CHECK(testNumber("1.1, ") == "1.100000");
        CHECK(testNumber("123,") == "123");
        CHECK(testNumber("123.1] ") == "123.100000");
        CHECK(testNumber("123]") == "123");
        CHECK(testNumber("123.22} ") == "123.220000");
        CHECK(testNumber("123}") == "123");
        CHECK(testNumber("123") == "123");
        CHECK(testNumber("123.22") == "123.220000");

        SUBCASE("Json_Error"){
            CHECK(testNumber("error") == "null");
            CHECK(testNumber("1.1.1") == "1.100000");
            CHECK(testNumber("0xb3") == "null");
            CHECK(testNumber("b123") == "null");
        }
    }
    SUBCASE("Json5"){
        JSON5 = true;

        CHECK(testNumber("123.22, ") == "123.220000");
        CHECK(testNumber("123,") == "123");
        CHECK(testNumber("123.22] ") == "123.220000");
        CHECK(testNumber("123]") == "123");
        CHECK(testNumber("123.22} ") == "123.220000");
        CHECK(testNumber("123}") == "123");
        CHECK(testNumber("123") == "123");
        CHECK(testNumber("123.22") == "123.220000");
        CHECK(testNumber("0xb3") == "0xb3");

        SUBCASE("Json5_Error"){
            CHECK(testNumber("error") == "null");
            CHECK(testNumber("1.1.1") == "1.100000");
            CHECK(testNumber("0xb3.123") == "0xb3");
            CHECK(testNumber("b123") == "null");
        }
    }
}

string testString(string str){
    auto json = jp(str).parseString();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_String"){
    SUBCASE("Json"){
        JSON5 = false;
        CHECK(testString(R"("'hello'\t\n")") == R"("'hello'\t\n")");
        CHECK(testString(R"("\"hello\"\t\n")") == R"("\"hello\"\t\n")");
        CHECK(testString(R"("a message")") == R"("a message")");
        CHECK(testString(R"("\u0022\u4f60\u597d\u002c\u0043\u002b\u002b\u0022")") == R"("\"你好,C++\"")");
        CHECK(testString(R"("\u0027\u4f60\u597d\u002c\u0043\u002b\u002b\u0027")") == R"("'你好,C++'")");
        CHECK(testString(R"("")") == R"("")");
        CHECK(testString(R"("\"\b\f\t\n\r\\")") == R"("\"\b\f\t\n\r\\")");

        SUBCASE("Json_Error"){
            CHECK(testString(R"("hello)") == R"(null)");
            CHECK(testString(R"("hello')") == R"(null)");
            CHECK(testString(R"('hello')") == R"(null)");
            CHECK(testString(R"('hello")") == R"(null)");
            CHECK(testString("\"hello\n\"") == R"(null)");
            CHECK(testString(R"("hello\'")") == R"(null)");
            CHECK(testString(R"("hell"o")") == R"("hell")");
            CHECK(testString(R"("hello\u002")") == R"(null)");
            CHECK(testString(R"("\hello")") == R"(null)");
            CHECK(testString(R"("hello\)") == R"(null)");
        }
    }
    SUBCASE("Json5"){
        JSON5 = true;
        CHECK(testString(R"("'hello'\t\n")") == R"('\'hello\'\t\n')");
        CHECK(testString(R"("\"hello\"\t\n")") == R"('"hello"\t\n')");
        CHECK(testString(R"("a message")") == R"('a message')");
        CHECK(testString(R"("\u0022\u4f60\u597d\u002c\u0043\u002b\u002b\u0022")") == R"('"你好,C++"')");
        CHECK(testString(R"("\u0027\u4f60\u597d\u002c\u0043\u002b\u002b\u0027")") == R"('\'你好,C++\'')");
        CHECK(testString(R"("")") == R"('')");
        CHECK(testString(R"("\"\b\f\t\n\r\\")") == R"('"\b\f\t\n\r\\')");
        CHECK(testString("\"hell\n\t  o\"") == R"('hello')");

        CHECK(testString(R"('\'hello\'\t\n')") == R"('\'hello\'\t\n')");
        CHECK(testString(R"('"hello"\t\n')") == R"('"hello"\t\n')");
        CHECK(testString(R"('a message')") == R"('a message')");
        CHECK(testString(R"('\u0022\u4f60\u597d\u002c\u0043\u002b\u002b\u0022')") == R"('"你好,C++"')");
        CHECK(testString(R"('\u0027\u4f60\u597d\u002c\u0043\u002b\u002b\u0027')") == R"('\'你好,C++\'')");
        CHECK(testString(R"('')") == R"('')");
        CHECK(testString(R"('"\b\f\t\n\r\\')") == R"('"\b\f\t\n\r\\')");
        CHECK(testString("'hell\n\t  o'") == R"('hello')");

        SUBCASE("Json5_Error"){
            CHECK(testString(R"("hello)") == R"(null)");
            CHECK(testString(R"("hello')") == R"(null)");
            CHECK(testString(R"('hello")") == R"(null)");
            CHECK(testString(R"("hello\'")") == R"(null)");
            CHECK(testString(R"("hell"o")") == R"('hell')");
            CHECK(testString(R"('hell'o')") == R"('hell')");
            CHECK(testString(R"("hello\u002")") == R"(null)");
            CHECK(testString(R"("\hello")") == R"(null)");
            CHECK(testString(R"("hello\)") == R"(null)");
            CHECK(testString(R"('\"hello')") == R"(null)");
        }
    }
}