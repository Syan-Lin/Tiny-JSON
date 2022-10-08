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
    auto after = jp::parse_string(i, str);
    cout << "original: " << str << endl;
    cout << "after: " << after.parse() << endl;
}

int main(){
    vector<string> tests;
    tests.push_back(R"("'hello'\t\n")");
    tests.push_back(R"("test\nnext\tline\n")");
    tests.push_back(R"("this is a message")");
    tests.push_back(R"("\u4f60\u597d\uff0c\u0043\u002b\u002b")");
    tests.push_back(R"("")");
    tests.push_back(R"("\"\b\f\t\n\r\\\u002E")");

    for(auto& e : tests){
        test(e);
        cout << endl;
    }

    JSON5 = true;
    vector<string> test5;
    test5.push_back(R"('\'hello
                        \t\n')");
    test5.push_back(R"('test\nnext\tline\n')");
    test5.push_back(R"('this is a message')");
    test5.push_back(R"('\u4f60\u597d\uff0c\u0043\u002b\u002b')");
    test5.push_back(R"('')");
    test5.push_back(R"('"\b\f\t\n\r\\\u002E')");

    test5.push_back(R"("'hello'\t\n")");
    test5.push_back(R"("test\nnext\tline\n")");
    test5.push_back(R"("this is a message")");
    test5.push_back(R"("\u4f60\u597d\uff0c\u0043\u002b\u002b")");
    test5.push_back(R"("")");
    test5.push_back(R"("\"\b\f\t\n\r\\\u002E")");

    for(auto& e : test5){
        test(e);
        cout << endl;
    }
}
