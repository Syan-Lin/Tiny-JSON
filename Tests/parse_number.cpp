#include <iostream>
#include <vector>
#include <codecvt>
#include <locale>
#include "../tiny_json.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

void test(string& str){
    int i = 0;
    auto after = jp::parse_number(i, str);
    cout << "original: " << str << endl;
    cout << "after: " << after.parse() << endl;
}

int main(){
    vector<string> tests;
    tests.push_back("123.22, ");
    tests.push_back("1234567, 891010");
    tests.push_back("12345678, 91010.123");
    tests.push_back("123.22");
    tests.push_back("1234567891010");
    tests.push_back("1234567891010.123");
    tests.push_back("123");

    for(auto& e : tests){
        test(e);
        cout << endl;
    }

    JSON5 = true;
    vector<string> test5;
    test5.push_back("0xabc123");

    for(auto& e : test5){
        test(e);
        cout << endl;
    }
}
