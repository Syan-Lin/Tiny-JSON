#include "tiny_json.h"
#include <iostream>

using namespace std;
using namespace tiny_json;

// 示例
void example(){
    // 1. 从字符串构造对象
    string json = R"(
        {
            "person": {
                "head": {
                    "eyes": "green",
                    "hair": {
                        "color": "lightblood",
                        "length": "short",
                        "style": "a-line"
                    }
                },
                "heightInInches": 66,
                "name": "LindsayBassett"
            }
        }
    )";
    Object obj = parse(json);
    auto& head = obj["person"]["head"];
    string& eyes_color = head["eyes"].get<string>();
    string& hair_style = head["hair"]["style"].get<string>();
    eyes_color = "blue";
    hair_style = "wavy";

    int& height = obj["person"]["heightInInches"].get<int>();
    height = 65;

    cout << parse(obj) << endl;

    // 2. 从 C++ 构造对象
    Object obj2 = {
        {"person", "bob"},
        {"happy", true},
        {"height", 1.83},
        {"hair", {
            {"color", "蓝色"},
            {"hairStyle", "短头发"}
        }},
        {"list", Array({1, 2, 3})}
    };
    cout << parse(obj2) << endl;
    writeFile("test.json", parse(obj2));
}

// JSON5 示例
void exampleJSON5(){
    // 1. 从字符串构造对象
    string json = R"(
        {
            // this is a personal data
            person: {
                head: {
                    eyes: 'green',
                    hair: {
                        color: 'lightblood',
                        length: 'short',
                        style: 'a-line'
                    }
                },
                heightInInches: 0x42,
                name: 'LindsayBassett'
            }
            /*
                This should not be showed.
            */
        }
    )";
    Object obj = parse(json);
    auto& head = obj["person"]["head"];
    string& eyes_color = head["eyes"].get<string>();
    string& hair_style = head["hair"]["style"].get<string>();
    eyes_color = "blue";
    hair_style = "wavy";

    int& height = obj["person"]["heightInInches"].get<int>();
    height = 65;

    cout << parse(obj) << endl;

    // 2. 从 C++ 构造对象
    Object obj2 = {
        {"person", "bob"},
        {"happy", true},
        {"height", 1.83},
        {"hair", {
            {"color", "blue"},
            {"hairStyle", "short"}
        }},
        {"list", Array({1, 2, 3})}
    };
    cout << parse(obj2) << endl;
}

int main(){
    JSON5 = false;      // 默认为 false
    example();

    JSON5 = true;
    exampleJSON5();
    return 0;
}