#include <iostream>
#include "../tiny_json.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include "ThirdParty/doctest.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

string test_arr(string str){
    auto json = jp(str).parse_array();
    return json.parse();
}
string test_obj(string str){
    auto json = jp(str).parse_object();
    return json.parse();
}

// Corverage: 100%
TEST_CASE("Parse_String"){
    detail = false;
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
