#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include "../tiny_json.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_COLORS_ANSI
#include "ThirdParty/doctest.h"
using namespace std;
using namespace tiny_json;
using namespace internal_class;

// Corverage: 100%
TEST_CASE("Class_Json_Test"){
    Json j;
    SUBCASE("null"){
        Json j_null(nullptr);
        CHECK(j.isNull());
        CHECK_FALSE(j.isArray());
        CHECK_FALSE(j.isBool());
        CHECK_FALSE(j.isNumber());
        CHECK_FALSE(j.isObject());
        CHECK_FALSE(j.isString());
        CHECK(j_null.isNull());
        CHECK_FALSE(j_null.isArray());
        CHECK_FALSE(j_null.isBool());
        CHECK_FALSE(j_null.isNumber());
        CHECK_FALSE(j_null.isObject());
        CHECK_FALSE(j_null.isString());
        CHECK(j.parse() == "null");
        CHECK(j_null.parse() == "null");
    }
    j = 1;
    SUBCASE("number"){
        Json j_num1(1);
        Json j_num2(1.1);
        CHECK(j.isNumber());
        CHECK_FALSE(j.isArray());
        CHECK_FALSE(j.isBool());
        CHECK_FALSE(j.isNull());
        CHECK_FALSE(j.isObject());
        CHECK_FALSE(j.isString());
        CHECK(j_num1.isNumber());
        CHECK_FALSE(j_num1.isArray());
        CHECK_FALSE(j_num1.isBool());
        CHECK_FALSE(j_num1.isNull());
        CHECK_FALSE(j_num1.isObject());
        CHECK_FALSE(j_num1.isString());
        CHECK(j_num2.isNumber());
        CHECK_FALSE(j_num2.isArray());
        CHECK_FALSE(j_num2.isBool());
        CHECK_FALSE(j_num2.isNull());
        CHECK_FALSE(j_num2.isObject());
        CHECK_FALSE(j_num2.isString());
        CHECK(j.getInt() == 1);
        CHECK(j_num1.getInt() == 1);
        CHECK(j_num2.getInt() == 1);
        CHECK(j.getDouble() == 1);
        CHECK(j_num1.getDouble() == 1);
        CHECK(j_num2.getDouble() == 1.1);
        CHECK(j.parse() == "1");
        CHECK(j_num1.parse() == "1");
        CHECK(j_num2.parse() == "1.100000");
        j_num1.hex(true);
        CHECK(j_num1.parse() == "0x1");
        j = 1.1;
        CHECK(j.getInt() == 1);
        CHECK(j.getDouble() == 1.1);
    }
    j = true;
    SUBCASE("bool"){
        Json j_bool1(true);
        Json j_bool2(false);
        CHECK(j.isBool());
        CHECK_FALSE(j.isArray());
        CHECK_FALSE(j.isNumber());
        CHECK_FALSE(j.isNull());
        CHECK_FALSE(j.isObject());
        CHECK_FALSE(j.isString());
        CHECK(j_bool1.isBool());
        CHECK_FALSE(j_bool1.isArray());
        CHECK_FALSE(j_bool1.isNumber());
        CHECK_FALSE(j_bool1.isNull());
        CHECK_FALSE(j_bool1.isObject());
        CHECK_FALSE(j_bool1.isString());
        CHECK(j.getBool() == true);
        CHECK(j_bool1.getBool() == true);
        CHECK(j_bool2.getBool() == false);
    }
    string temp = "temp";
    j = temp;
    SUBCASE("string"){
        string hello = "hello";
        Json j_string(hello);
        Json j_string2(move(hello));
        Json j_string3("hello");
        CHECK(j.isString());
        CHECK_FALSE(j.isArray());
        CHECK_FALSE(j.isNumber());
        CHECK_FALSE(j.isNull());
        CHECK_FALSE(j.isObject());
        CHECK_FALSE(j.isBool());
        CHECK(j_string.isString());
        CHECK_FALSE(j_string.isArray());
        CHECK_FALSE(j_string.isNumber());
        CHECK_FALSE(j_string.isNull());
        CHECK_FALSE(j_string.isObject());
        CHECK_FALSE(j_string.isBool());
        CHECK(j_string2.isString());
        CHECK_FALSE(j_string2.isArray());
        CHECK_FALSE(j_string2.isNumber());
        CHECK_FALSE(j_string2.isNull());
        CHECK_FALSE(j_string2.isObject());
        CHECK_FALSE(j_string2.isBool());
        CHECK(j_string3.isString());
        CHECK_FALSE(j_string3.isArray());
        CHECK_FALSE(j_string3.isNumber());
        CHECK_FALSE(j_string3.isNull());
        CHECK_FALSE(j_string3.isObject());
        CHECK_FALSE(j_string3.isBool());
        CHECK(j.getString() == "temp");
        j = "hello";
        CHECK(j.getString() == "hello");
        CHECK(j_string.getString() == "hello");
        CHECK(j_string2.getString() == "hello");
        CHECK(j_string3.getString() == "hello");
        j = string("hello");
        CHECK(j.getString() == "hello");
    }
    SUBCASE("json"){
        Json temp;
        j = temp;
        CHECK(j.isNull());
        CHECK_FALSE(j.isArray());
        CHECK_FALSE(j.isBool());
        CHECK_FALSE(j.isNumber());
        CHECK_FALSE(j.isObject());
        CHECK_FALSE(j.isString());
        j = Json(1);
        CHECK(j.isNumber());
        CHECK_FALSE(j.isArray());
        CHECK_FALSE(j.isBool());
        CHECK_FALSE(j.isNull());
        CHECK_FALSE(j.isObject());
        CHECK_FALSE(j.isString());
        CHECK(j.getInt() == 1);
        Json copy(j);
        CHECK(copy.isNumber());
        CHECK_FALSE(copy.isArray());
        CHECK_FALSE(copy.isBool());
        CHECK_FALSE(copy.isNull());
        CHECK_FALSE(copy.isObject());
        CHECK_FALSE(copy.isString());
        CHECK(copy.getInt() == 1);
        Json m(move(copy));
        CHECK(m.isNumber());
        CHECK_FALSE(m.isArray());
        CHECK_FALSE(m.isBool());
        CHECK_FALSE(m.isNull());
        CHECK_FALSE(m.isObject());
        CHECK_FALSE(m.isString());
        CHECK(m.getInt() == 1);
    }
    j = Array({1, 1, 1});
    SUBCASE("array"){
        Array test({2, 2, 2});
        Json arr1(test);
        Json arr2(move(test));
        CHECK(j.isArray());
        CHECK_FALSE(j.isNull());
        CHECK_FALSE(j.isBool());
        CHECK_FALSE(j.isNumber());
        CHECK_FALSE(j.isObject());
        CHECK_FALSE(j.isString());
        CHECK(arr1.isArray());
        CHECK_FALSE(arr1.isNull());
        CHECK_FALSE(arr1.isBool());
        CHECK_FALSE(arr1.isNumber());
        CHECK_FALSE(arr1.isObject());
        CHECK_FALSE(arr1.isString());
        CHECK(arr2.isArray());
        CHECK_FALSE(arr2.isNull());
        CHECK_FALSE(arr2.isBool());
        CHECK_FALSE(arr2.isNumber());
        CHECK_FALSE(arr2.isObject());
        CHECK_FALSE(arr2.isString());
        for(auto& e : j.getArray()){
            CHECK(e.getInt() == 1);
        }
        for(auto& e : arr1.getArray()){
            CHECK(e.getInt() == 2);
        }
        for(auto& e : arr2.getArray()){
            CHECK(e.getInt() == 2);
        }
        j = test;
        for(auto& e : j.getArray()){
            CHECK(e.getInt() == 2);
        }
        SUBCASE("out_of_boundary"){
            // if out of boundary, create null automatically {2, 2, 2} -> {2, 2, 2, null, 1}
            j[4] = 1;
            CHECK(j[3].isNull());
            CHECK(j[4].getInt() == 1);
        }
    }
    j = Object({{"age", 18}, {"num", 18}});
    SUBCASE("object"){
        Object test({{"age", 20}, {"num", 20}});
        Json obj1(test);
        Json obj2(move(test));
        CHECK(j.isObject());
        CHECK_FALSE(j.isNull());
        CHECK_FALSE(j.isBool());
        CHECK_FALSE(j.isNumber());
        CHECK_FALSE(j.isArray());
        CHECK_FALSE(j.isString());
        CHECK(obj1.isObject());
        CHECK_FALSE(obj1.isNull());
        CHECK_FALSE(obj1.isBool());
        CHECK_FALSE(obj1.isNumber());
        CHECK_FALSE(obj1.isArray());
        CHECK_FALSE(obj1.isString());
        CHECK(obj2.isObject());
        CHECK_FALSE(obj2.isNull());
        CHECK_FALSE(obj2.isBool());
        CHECK_FALSE(obj2.isNumber());
        CHECK_FALSE(obj2.isArray());
        CHECK_FALSE(obj2.isString());
        for(auto& e : j.getObject()){
            CHECK(e.second.getInt() == 18);
        }
        for(auto& e : obj1.getObject()){
            CHECK(e.second.getInt() == 20);
        }
        for(auto& e : obj2.getObject()){
            CHECK(e.second.getInt() == 20);
        }
        j = test;
        for(auto& e : j.getObject()){
            CHECK(e.second.getInt() == 20);
        }
        SUBCASE("out_of_boundary"){
            j["other"] = 18;
            CHECK(j["other"].getInt() == 18);
        }
    }
    j.reset();
    CHECK(j.isNull());
    SUBCASE("extension"){
        class Test{
        public:
            Test(int num) : i(num) {}
            int i;
            int json() const {
                return i;
            }
        };
        Test t(2);
        Json ext(t);
        CHECK(ext.getInt() == 2);
        unordered_map<string, double> m({{"length", 3.14}, {"height", 3.14}});
        Json ext2(m);
        for(auto& e : ext2.getObject()){
            CHECK(e.second.getDouble() == 3.14);
        }
        list<int> l({1, 1, 1});
        Json ext3(l);
        for(auto& e : ext2.getArray()){
            CHECK(e.getInt() == 1);
        }
    }
}

// Corverage: 100%
TEST_CASE("Class_JsonBoolean_Test"){
    SUBCASE("bool_true"){
        JsonBoolean jb(true);
        CHECK(jb.getArray().empty());
        CHECK(jb.getObject().empty());
        CHECK(jb.getString().empty());
        CHECK(jb.getBool() == true);
        CHECK(jb.getInt() == 0);
        CHECK(jb.getDouble() == 0);
        CHECK(jb[1].parse() == "null");
        CHECK(jb["key"].parse() == "null");
    }
    SUBCASE("bool_false"){
        JsonBoolean jb(false);
        CHECK(jb.getArray().empty());
        CHECK(jb.getObject().empty());
        CHECK(jb.getString().empty());
        CHECK(jb.getBool() == false);
        CHECK(jb.getInt() == 0);
        CHECK(jb.getDouble() == 0);
        CHECK(jb[1].parse() == "null");
        CHECK(jb["key"].parse() == "null");
    }
}

// Corverage: 100%
TEST_CASE("Class_JsonNumber_Test"){
    int num1 = 314;
    double num2 = 3.14;
    JsonNumber jn1(num1), jn2(num2);
    CHECK(jn1.getDouble() == 314);
    CHECK(jn1.getInt() == 314);
    CHECK(jn2.getDouble() == 3.14);
    CHECK(jn2.getInt() == 3);
    jn1 = 1.1;
    jn2 = 1;
    CHECK(jn1.getDouble() == 1.1);
    CHECK(jn1.getInt() == 1);
    CHECK(jn2.getDouble() == 1);
    CHECK(jn2.getInt() == 1);
    string temp;
    jn1.parse(temp);
    CHECK(temp == "1.100000");
    temp.clear();
    jn2.parse(temp);
    CHECK(temp == "1");
    SUBCASE("hex"){
        jn1.hex = true;
        jn2.hex = true;
        temp.clear();
        jn1.parse(temp);
        CHECK(temp == "1.100000");
        temp.clear();
        jn2.parse(temp);
        CHECK(temp == "0x1");
    }
}

// Corverage: 100%
TEST_CASE("Class_JsonString_Test"){
    string hello = "hello";
    JsonString js1(hello);
    JsonString js2(move(hello));
    SUBCASE("js1"){
        CHECK(js1.getString() == "hello");
        string temp = "temp";
        js1 = temp;
        CHECK(js1.getString() == "temp");
        js1 = "temp";
        CHECK(js1.getString() == "temp");
    }
    SUBCASE("js2"){
        CHECK(js2.getString() == "hello");
        string temp = "temp";
        js2 = temp;
        CHECK(js2.getString() == "temp");
        js2 = "temp";
        CHECK(js2.getString() == "temp");
    }
}

// Corverage: 100%
TEST_CASE("Class_JsonArray_Test"){
    Array test({1, true, nullptr, "string", {{"key", "value"}}});
    JsonArray ja1(test);
    JsonArray ja2(move(test));
    SUBCASE("element_test"){
        CHECK(ja1[0].parse() == "1");
        CHECK(ja1[1].parse() == "true");
        CHECK(ja1[2].parse() == "null");
        CHECK(ja1[3].parse() == "\"string\"");
        CHECK(ja1[4].parse() == R"({"key":"value"})");
        CHECK(ja2[0].parse() == "1");
        CHECK(ja2[1].parse() == "true");
        CHECK(ja2[2].parse() == "null");
        CHECK(ja2[3].parse() == "\"string\"");
        CHECK(ja2[4].parse() == R"({"key":"value"})");
    }
    Array test2({1, 1, 1});
    ja1 = test2;
    ja2 = move(test2);
    SUBCASE("ergodic_test"){
        for(auto& e : ja1.getArray()){
            CHECK(e.parse() == "1");
        }
        for(auto& e : ja2.getArray()){
            CHECK(e.parse() == "1");
        }
    }
    SUBCASE("quote_test"){
        Array& quote = ja1.getVec();
        for(auto& e : quote){
            e = 2;
        }
        for(auto& e : ja1.getArray()){
            CHECK(e.parse() == "2");
        }
    }
    SUBCASE("out_of_boundary"){
        try{
            ja1[10] = 1;
        }catch(exception &e) {}
    }
}

// Corverage: 100%
TEST_CASE("Class_JsonObject_Test"){
    Object test({{"name", "bob"}, {"age", 18}});
    JsonObject jo1(test), jo2(move(test));
    SUBCASE("element_test"){
        CHECK(jo1["name"].getString() == "bob");
        CHECK(jo1["age"].getInt() == 18);
        CHECK(jo1["name"].getString() == "bob");
        CHECK(jo1["age"].getInt() == 18);
    }
    Object test2({{"age", 18}, {"number", 18}});
    jo1 = test2;
    jo2 = move(test2);
    SUBCASE("ergodic_test"){
        for(auto& e : jo1.getObject()){
            CHECK(e.second.getInt() == 18);
        }
        for(auto& e : jo2.getObject()){
            CHECK(e.second.getInt() == 18);
        }
    }
    SUBCASE("quote_test"){
        Object& quote = jo1.getMap();
        for(auto& e : quote){
            e.second = 2;
        }
        for(auto& e : quote){
            CHECK(e.second.getInt() == 2);
        }
    }
    SUBCASE("out_of_boundary"){
        jo1["temp"] = 1;
        CHECK(jo1["temp"].getInt() == 1);
    }
}
