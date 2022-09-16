# Tiny-JSON
![](https://img.shields.io/badge/c%2B%2B-11-blue) ![](https://img.shields.io/badge/release-v1.0-blue) ![](https://img.shields.io/badge/coverage-100%25-green) ![](https://img.shields.io/badge/license-mit-blue)

### Introduction
Tiny-JSON is a light-weight JSON liberary implemented by C++11, only one head file needed to import projects. The coding style refers to Google C++ Style Guide, and have rich annotations, no third party dependency.

[中文版本](https://github.com/Syan-Lin/Tiny-JSON/blob/main/README.md)

### Features
- Read JSON file and parse to C++ objects
- Use C++ objects to generate JSON file
- Support JSON5
- Extremly easy used

### Examples
Compiler version: `gcc version 8.1.0 (x86_64-posix-seh-rev0)`

```cpp
#include "tiny_json.h"
using namespace tiny_json;

// 1. Read JSON file and parse to C++ objects
string json = R"({
        "integer": 1,
        "string": "Hello World!",
        "array": [
            1.2,
            "string",
            false
        ]
        })";
Object obj = parse(json);

int integer = obj["integer"].get<int>();
string str = obj["string"].get<string>();
double first = obj["array"][0].get<double>();
string second = obj["array"][1].get<string>();
bool third = obj["array"][2].get<bool>();
obj["array"][2] = "change to string";

// 2. Use C++ objects to generate JSON string
Object obj2 = {
    {"number", 5.2},
    {"string", "Hello World!"},
    {"bool", false},
    {"array", Array({1, false, "abc"})}
};
// Generate JSON string
string json2 = parse(obj2);
```

>More examples in `tiny_json_example.cpp` and `tiny_json_test.cpp`

### UML Diagram
![uml](uml.jpg)

### TODO
- [x] JSON5 fully supported
- [ ] Normal pointer version
