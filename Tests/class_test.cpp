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
TEST_CASE("Class_JsonBoolean_Test"){
    SUBCASE("bool_true"){
        JsonBoolean jb(true);
        CHECK(jb.get_array().empty());
        CHECK(jb.get_object().empty());
        CHECK(jb.get_string().empty());
        CHECK(jb.get_bool() == true);
        CHECK(jb.get_int() == 0);
        CHECK(jb.get_double() == 0);
        CHECK(jb[1].parse() == "null");
        CHECK(jb["key"].parse() == "null");
    }
    SUBCASE("bool_false"){
        JsonBoolean jb(false);
        CHECK(jb.get_array().empty());
        CHECK(jb.get_object().empty());
        CHECK(jb.get_string().empty());
        CHECK(jb.get_bool() == false);
        CHECK(jb.get_int() == 0);
        CHECK(jb.get_double() == 0);
        CHECK(jb[1].parse() == "null");
        CHECK(jb["key"].parse() == "null");
    }
}

// Corverage: 100%
TEST_CASE("Class_JsonNumber_Test"){
    int num1 = 314;
    double num2 = 3.14;
    JsonNumber jn1(num1), jn2(num2);
    CHECK(jn1.get_double() == 314);
    CHECK(jn1.get_int() == 314);
    CHECK(jn2.get_double() == 3.14);
    CHECK(jn2.get_int() == 3);
    jn1 = 1.1;
    jn2 = 1;
    CHECK(jn1.get_double() == 1.1);
    CHECK(jn1.get_int() == 1);
    CHECK(jn2.get_double() == 1);
    CHECK(jn2.get_int() == 1);
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
        CHECK(js1.get_string() == "hello");
        string temp = "temp";
        js1 = temp;
        CHECK(js1.get_string() == "temp");
        js1 = "temp";
        CHECK(js1.get_string() == "temp");
    }
    SUBCASE("js2"){
        CHECK(js2.get_string() == "hello");
        string temp = "temp";
        js2 = temp;
        CHECK(js2.get_string() == "temp");
        js2 = "temp";
        CHECK(js2.get_string() == "temp");
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
        for(auto& e : ja1.get_array()){
            CHECK(e.parse() == "1");
        }
        for(auto& e : ja2.get_array()){
            CHECK(e.parse() == "1");
        }
    }
    SUBCASE("quote_test"){
        Array& quote = ja1.get_vec();
        for(auto& e : quote){
            e = 2;
        }
        for(auto& e : ja1.get_array()){
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
        CHECK(jo1["name"].get_string() == "bob");
        CHECK(jo1["age"].get_int() == 18);
        CHECK(jo1["name"].get_string() == "bob");
        CHECK(jo1["age"].get_int() == 18);
    }
    Object test2({{"age", 18}, {"number", 18}});
    jo1 = test2;
    jo2 = move(test2);
    SUBCASE("ergodic_test"){
        for(auto& e : jo1.get_object()){
            CHECK(e.second.get_int() == 18);
        }
        for(auto& e : jo2.get_object()){
            CHECK(e.second.get_int() == 18);
        }
    }
    SUBCASE("quote_test"){
        Object& quote = jo1.get_map();
        for(auto& e : quote){
            e.second = 2;
        }
        for(auto& e : quote){
            CHECK(e.second.get_int() == 2);
        }
    }
    SUBCASE("out_of_boundary"){
        jo1["temp"] = 1;
        CHECK(jo1["temp"].get_int() == 1);
    }
}

// Corverage: 100%
TEST_CASE("Class_Json_Test"){
    Json j;
    SUBCASE("null"){
        Json j_null(nullptr);
        CHECK(j.is_null());
        CHECK_FALSE(j.is_array());
        CHECK_FALSE(j.is_bool());
        CHECK_FALSE(j.is_number());
        CHECK_FALSE(j.is_object());
        CHECK_FALSE(j.is_string());
        CHECK(j_null.is_null());
        CHECK_FALSE(j_null.is_array());
        CHECK_FALSE(j_null.is_bool());
        CHECK_FALSE(j_null.is_number());
        CHECK_FALSE(j_null.is_object());
        CHECK_FALSE(j_null.is_string());
        CHECK(j.parse() == "null");
        CHECK(j_null.parse() == "null");
    }
    j = 1;
    SUBCASE("number"){
        Json j_num1(1);
        Json j_num2(1.1);
        CHECK(j.is_number());
        CHECK_FALSE(j.is_array());
        CHECK_FALSE(j.is_bool());
        CHECK_FALSE(j.is_null());
        CHECK_FALSE(j.is_object());
        CHECK_FALSE(j.is_string());
        CHECK(j_num1.is_number());
        CHECK_FALSE(j_num1.is_array());
        CHECK_FALSE(j_num1.is_bool());
        CHECK_FALSE(j_num1.is_null());
        CHECK_FALSE(j_num1.is_object());
        CHECK_FALSE(j_num1.is_string());
        CHECK(j_num2.is_number());
        CHECK_FALSE(j_num2.is_array());
        CHECK_FALSE(j_num2.is_bool());
        CHECK_FALSE(j_num2.is_null());
        CHECK_FALSE(j_num2.is_object());
        CHECK_FALSE(j_num2.is_string());
        CHECK(j.get_int() == 1);
        CHECK(j_num1.get_int() == 1);
        CHECK(j_num2.get_int() == 1);
        CHECK(j.get_double() == 1);
        CHECK(j_num1.get_double() == 1);
        CHECK(j_num2.get_double() == 1.1);
        CHECK(j.parse() == "1");
        CHECK(j_num1.parse() == "1");
        CHECK(j_num2.parse() == "1.100000");
        j_num1.hex(true);
        CHECK(j_num1.parse() == "0x1");
        j = 1.1;
        CHECK(j.get_int() == 1);
        CHECK(j.get_double() == 1.1);
    }
    j = true;
    SUBCASE("bool"){
        Json j_bool1(true);
        Json j_bool2(false);
        CHECK(j.is_bool());
        CHECK_FALSE(j.is_array());
        CHECK_FALSE(j.is_number());
        CHECK_FALSE(j.is_null());
        CHECK_FALSE(j.is_object());
        CHECK_FALSE(j.is_string());
        CHECK(j_bool1.is_bool());
        CHECK_FALSE(j_bool1.is_array());
        CHECK_FALSE(j_bool1.is_number());
        CHECK_FALSE(j_bool1.is_null());
        CHECK_FALSE(j_bool1.is_object());
        CHECK_FALSE(j_bool1.is_string());
        CHECK(j.get_bool() == true);
        CHECK(j_bool1.get_bool() == true);
        CHECK(j_bool2.get_bool() == false);
    }
    string temp = "temp";
    j = temp;
    SUBCASE("string"){
        string hello = "hello";
        Json j_string(hello);
        Json j_string2(move(hello));
        Json j_string3("hello");
        CHECK(j.is_string());
        CHECK_FALSE(j.is_array());
        CHECK_FALSE(j.is_number());
        CHECK_FALSE(j.is_null());
        CHECK_FALSE(j.is_object());
        CHECK_FALSE(j.is_bool());
        CHECK(j_string.is_string());
        CHECK_FALSE(j_string.is_array());
        CHECK_FALSE(j_string.is_number());
        CHECK_FALSE(j_string.is_null());
        CHECK_FALSE(j_string.is_object());
        CHECK_FALSE(j_string.is_bool());
        CHECK(j_string2.is_string());
        CHECK_FALSE(j_string2.is_array());
        CHECK_FALSE(j_string2.is_number());
        CHECK_FALSE(j_string2.is_null());
        CHECK_FALSE(j_string2.is_object());
        CHECK_FALSE(j_string2.is_bool());
        CHECK(j_string3.is_string());
        CHECK_FALSE(j_string3.is_array());
        CHECK_FALSE(j_string3.is_number());
        CHECK_FALSE(j_string3.is_null());
        CHECK_FALSE(j_string3.is_object());
        CHECK_FALSE(j_string3.is_bool());
        CHECK(j.get_string() == "temp");
        j = "hello";
        CHECK(j.get_string() == "hello");
        CHECK(j_string.get_string() == "hello");
        CHECK(j_string2.get_string() == "hello");
        CHECK(j_string3.get_string() == "hello");
        j = string("hello");
        CHECK(j.get_string() == "hello");
    }
    SUBCASE("json"){
        Json temp;
        j = temp;
        CHECK(j.is_null());
        CHECK_FALSE(j.is_array());
        CHECK_FALSE(j.is_bool());
        CHECK_FALSE(j.is_number());
        CHECK_FALSE(j.is_object());
        CHECK_FALSE(j.is_string());
        j = Json(1);
        CHECK(j.is_number());
        CHECK_FALSE(j.is_array());
        CHECK_FALSE(j.is_bool());
        CHECK_FALSE(j.is_null());
        CHECK_FALSE(j.is_object());
        CHECK_FALSE(j.is_string());
        CHECK(j.get_int() == 1);
        Json copy(j);
        CHECK(copy.is_number());
        CHECK_FALSE(copy.is_array());
        CHECK_FALSE(copy.is_bool());
        CHECK_FALSE(copy.is_null());
        CHECK_FALSE(copy.is_object());
        CHECK_FALSE(copy.is_string());
        CHECK(copy.get_int() == 1);
        Json m(move(copy));
        CHECK(m.is_number());
        CHECK_FALSE(m.is_array());
        CHECK_FALSE(m.is_bool());
        CHECK_FALSE(m.is_null());
        CHECK_FALSE(m.is_object());
        CHECK_FALSE(m.is_string());
        CHECK(m.get_int() == 1);
    }
    j = Array({1, 1, 1});
    SUBCASE("array"){
        Array test({2, 2, 2});
        Json arr1(test);
        Json arr2(move(test));
        CHECK(j.is_array());
        CHECK_FALSE(j.is_null());
        CHECK_FALSE(j.is_bool());
        CHECK_FALSE(j.is_number());
        CHECK_FALSE(j.is_object());
        CHECK_FALSE(j.is_string());
        CHECK(arr1.is_array());
        CHECK_FALSE(arr1.is_null());
        CHECK_FALSE(arr1.is_bool());
        CHECK_FALSE(arr1.is_number());
        CHECK_FALSE(arr1.is_object());
        CHECK_FALSE(arr1.is_string());
        CHECK(arr2.is_array());
        CHECK_FALSE(arr2.is_null());
        CHECK_FALSE(arr2.is_bool());
        CHECK_FALSE(arr2.is_number());
        CHECK_FALSE(arr2.is_object());
        CHECK_FALSE(arr2.is_string());
        for(auto& e : j.get_array()){
            CHECK(e.get_int() == 1);
        }
        for(auto& e : arr1.get_array()){
            CHECK(e.get_int() == 2);
        }
        for(auto& e : arr2.get_array()){
            CHECK(e.get_int() == 2);
        }
        j = test;
        for(auto& e : j.get_array()){
            CHECK(e.get_int() == 2);
        }
        SUBCASE("out_of_boundary"){
            // 如果越界，自动创建 null {2, 2, 2} -> {2, 2, 2, null, 1}
            j[4] = 1;
            CHECK(j[3].is_null());
            CHECK(j[4].get_int() == 1);
        }
    }
    j = Object({{"age", 18}, {"num", 18}});
    SUBCASE("object"){
        Object test({{"age", 20}, {"num", 20}});
        Json obj1(test);
        Json obj2(move(test));
        CHECK(j.is_object());
        CHECK_FALSE(j.is_null());
        CHECK_FALSE(j.is_bool());
        CHECK_FALSE(j.is_number());
        CHECK_FALSE(j.is_array());
        CHECK_FALSE(j.is_string());
        CHECK(obj1.is_object());
        CHECK_FALSE(obj1.is_null());
        CHECK_FALSE(obj1.is_bool());
        CHECK_FALSE(obj1.is_number());
        CHECK_FALSE(obj1.is_array());
        CHECK_FALSE(obj1.is_string());
        CHECK(obj2.is_object());
        CHECK_FALSE(obj2.is_null());
        CHECK_FALSE(obj2.is_bool());
        CHECK_FALSE(obj2.is_number());
        CHECK_FALSE(obj2.is_array());
        CHECK_FALSE(obj2.is_string());
        for(auto& e : j.get_object()){
            CHECK(e.second.get_int() == 18);
        }
        for(auto& e : obj1.get_object()){
            CHECK(e.second.get_int() == 20);
        }
        for(auto& e : obj2.get_object()){
            CHECK(e.second.get_int() == 20);
        }
        j = test;
        for(auto& e : j.get_object()){
            CHECK(e.second.get_int() == 20);
        }
        SUBCASE("out_of_boundary"){
            j["other"] = 18;
            CHECK(j["other"].get_int() == 18);
        }
    }
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
        CHECK(ext.get_int() == 2);
        unordered_map<string, double> m({{"length", 3.14}, {"height", 3.14}});
        Json ext2(m);
        for(auto& e : ext2.get_object()){
            CHECK(e.second.get_double() == 3.14);
        }
        list<int> l({1, 1, 1});
        Json ext3(l);
        for(auto& e : ext2.get_array()){
            CHECK(e.get_int() == 1);
        }
    }
}
