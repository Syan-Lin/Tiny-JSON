#include <iostream>
#include <vector>
#include <codecvt>
#include <locale>
#include "../tiny_json.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

void test_bool(string& str){
    int i = 0;
    Json after;
    if(str[i] == 't'){
        after = jp::parse_true(i, str);
    }else{
        after = jp::parse_false(i, str);
    }
    cout << "original: " << str << endl;
    cout << "after: " << after.parse() << endl;
}

void test_null(string& str){
    int i = 0;
    auto after = jp::parse_null(i, str);
    cout << "original: " << str << endl;
    after.parse();
    cout << "after: " << after.parse() << endl;
}

int main(){
    vector<string> tests;
    tests.push_back("false");
    tests.push_back("true");
    tests.push_back("error");

    vector<string> testn;
    testn.push_back("null");
    testn.push_back("nullr");

    for(auto& e : tests){
        test_bool(e);
        cout << endl;
    }
    for(auto& e : testn){
        test_null(e);
        cout << endl;
    }
}
