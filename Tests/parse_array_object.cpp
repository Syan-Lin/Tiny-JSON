#include <iostream>
#include <vector>
#include "../tiny_json.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

void test_arr(string& str){
    int i = 0;
    auto after = jp::parse_array(i, str);
    cout << "original: " << str << endl;
    cout << "after: " << after.parse() << endl;
}
void test_obj(string& str){
    int i = 0;
    auto after = jp::parse_object(i, str);
    cout << "original: " << str << endl;
    cout << "after: " << after.parse() << endl;
}

int main(){
    vector<string> arr;
    arr.push_back(R"([1, "abc",    null,
                    false, true])");
    arr.push_back(R"([[1,1],["a","a"],[true,false],[null,null]])");
    arr.push_back(R"([[[    123,12  3],true
                ],[false,["ab   c","a  bc"]]  , [true,false] , [null,null]])");
    arr.push_back(R"([{"str" : "hello", "number" : [123, 234]},[123, 24],{"obj":{"name":"joe"}}])");

    vector<string> obj;
    obj.push_back(R"({"str" : "hello", "number" : 123})");
    obj.push_back(R"({"str":"hello","number":123})");
    obj.push_back(R"({"str":{"str":"jane","arr":[1,2,3]},"number":123,"ha":null})");
    obj.push_back(R"({"str"  :
                    {"str"  :
                    "jane",  "arr"
                    :  [1,2,3]}  ,  "number" : 123, "ha" : null })");

    for(auto& e : arr){
        test_arr(e);
        cout << endl;
    }

    for(auto& e : obj){
        test_obj(e);
        cout << endl;
    }

    JSON5 = true;
    vector<string> arr5;
    arr5.push_back(R"([[1,1,],["a","a",],[true,false,],[null,null,],])");

    vector<string> obj5;

    for(auto& e : arr5){
        test_arr(e);
        cout << endl;
    }

    for(auto& e : obj5){
        test_obj(e);
        cout << endl;
    }
}
