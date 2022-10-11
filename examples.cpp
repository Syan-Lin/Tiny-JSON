#include <iostream>
#include "tiny_json.h"

using namespace std;
using namespace tiny_json;

void example(){
    // 1. Initial a json object
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
    Json obj = parse(json);
    auto& head = obj["person"]["head"];
    auto& eyes = head["eyes"];
    auto& hair_style = head["hair"]["style"];
    cout << "eyes_color: " << eyes.getString() << endl;
    cout << "hair_style: " << hair_style.getString() << endl;

    // 2. Change the value of json object
    eyes = "blue";
    hair_style = "straight";

    // 3. Construct a json object(format is optional)
    Json obj2 = {
        {"person", "bob"},
        {"happy", true},
        {"height", 1.83},
        {"hair", {
            {"color", "blue"},
            {"hairStyle", "short"}
        }},
        {"list", Array({1, 2, 3})}
    };
    cout << format(obj2.parse()) << endl;
}

void example_json5(){
    // 1. Initial a json5 object
    JSON5 = true;
    string json = R"(
        {
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
        }
    )";
    Json obj = parse(json);
    auto& head = obj["person"]["head"];
    auto& eyes = head["eyes"];
    auto& hair_style = head["hair"]["style"];
    cout << "eyes_color: " << eyes.getString() << endl;
    cout << "hair_style: " << hair_style.getString() << endl;

    // 2. Change the value of json object
    eyes = "blue";
    hair_style = "straight";

    // 3. Construct a json object(format is optional)
    Json obj2 = {
        {"person", "bob"},
        {"happy", true},
        {"height", 1.83},
        {"hair", {
            {"color", "blue"},
            {"hairStyle", "short"}
        }},
        {"list", Array({1, 2, 3})}
    };
    cout << format(obj2.parse()) << endl;
    JSON5 = false;
}

void error_test(){
    string json = R"({
            "hair": {
                // an error ouccrs in line 4
                "color", "lightblood",
                "length": "short",
                "style": "a-line"
            }
        })";
    // Will print [TinyJSON] expect ':' but get ',' in line: 4
    Json obj = parse(json);
    // Will not print but write information to err
    string err;
    detail = false;
    Json obj2 = parse(json ,err);
}

class Person{
public:
    Person(int height, int weight, string name)
    : m_height(height), m_weight(weight), m_name(name) {}
    int m_height;
    int m_weight;
    string m_name;
public:
    Json json(){
        return {
            {"height", m_height},
            {"weight", m_weight},
            {"name", m_name}
        };
    }
};

void ext_test(){
    Person tom(180, 70, "tom");
    Person bob(170, 60, "bob");
    Person jane(160, 50, "jane");
    Json json;
    json["person_info"] = Array({tom, bob, jane});
    cout << format(json.parse()) << endl;
}

int main() {
    example();
    example_json5();
    error_test();
    ext_test();
    return 0;
}