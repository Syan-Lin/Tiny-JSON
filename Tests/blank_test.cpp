#include <iostream>
#include <vector>
#include "../tiny_json.h"
using namespace std;
using namespace tiny_json;
using jp = json_parser::JsonParser;

void test(string& str){
    string after;
    for(int i = 0; i < str.size(); i++){
        jp::skip_blanks(i, str);
        after += str[i];
    }
    cout << "original: " << str << endl;
    cout << "after: " << after << endl;
}

int main(){
    vector<string> tests;
    tests.push_back("information  \n    ");
    tests.push_back("in  form  a\n \n  t    io      n");

    for(auto& e : tests){
        test(e);
        cout << endl;
    }
}
