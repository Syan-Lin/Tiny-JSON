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
