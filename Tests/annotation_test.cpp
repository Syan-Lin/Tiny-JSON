#include <iostream>
#include <vector>
#include "../tiny_json.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

void test(string& str){
    string after;
    int i = 0;
    for(; i < str.size(); i++){
        jp::skip_annotation(i, str);
        if(i >= str.size()) break;
        after += str[i];
    }
    cout << "str_size: " << str.size() << " i: " << i << endl;
    cout << "original: " << str << endl;
    cout << "after: " << after << endl;
}

int main(){
    vector<string> tests;
    tests.push_back("information/error");
    tests.push_back("information/");
    tests.push_back("information//annotation\ninformation");
    tests.push_back("//annotation\ninformation");
    tests.push_back("information//");
    tests.push_back("error/*");
    tests.push_back("error/**");
    tests.push_back("error/**/");
    tests.push_back("/*error");
    tests.push_back("error/*error");
    tests.push_back("information/*annotation*/");
    tests.push_back("information/*annotation*/information");
    tests.push_back("information/*\nannotation\n*/");

    for(auto& e : tests){
        test(e);
        cout << endl;
    }
}
