# Tiny-JSON
![](https://img.shields.io/badge/c%2B%2B-11-blue) ![](https://img.shields.io/badge/release-v1.2-blue) ![](https://img.shields.io/badge/coverage-100%25-green) ![](https://img.shields.io/badge/license-mit-blue)

### Introduction
Tiny-JSON is a light-weight JSON liberary implemented by C++11, only one head file needed to import projects. The coding style refers to Google C++ Style Guide, and have rich annotations, no third party dependency.

[中文版本](https://github.com/Syan-Lin/Tiny-JSON/blob/main/README.md)

### Features
- Read JSON file and parse to C++ objects
- Use C++ objects to generate JSON file
- Highly readable code
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

### Tests
#### Cases Tests
The test frame defined some simple asserts, which are used to judge bool value, number or string. When a assert is false, it will print corresponding code infomation, like:
```
============================================
Run Test: Test
Expected <2> but get <1> in D:\Tiny-JSON\Test-Frame\tiny_json_test.cpp at line: 871
Expected <true> but get <false> in D:\Tiny-JSON\Test-Frame\tiny_json_test.cpp at line: 872
Expected <Hello!> but get <Hello> in D:\Tiny-JSON\Test-Frame\tiny_json_test.cpp at line: 873
0/3 Cases passed. (0%)
Test failed!
==================Test End==================
```
You can show more assert infomation with command `tiny_json_test::Test::show_details_ = true`
```
============================================
Run Test: Test
Expected <2> but get <1> in D:\Tiny-JSON\Test-Frame\tiny_json_test.cpp at line: 871
Expected <true> but get <false> in D:\Tiny-JSON\Test-Frame\tiny_json_test.cpp at line: 872
Expected <Hello!> but get <Hello> in D:\Tiny-JSON\Test-Frame\tiny_json_test.cpp at line: 873
ExpectInt Check!
ExpectFalse Check!
ExpectString Check!
3/6 Cases passed. (50%)
Test failed!
==================Test End==================
```
#### Performance tests
Performance tests are defined in `tiny_json_performance.h`, which could set the scale of types separately to see its actual performance.

Test cases defined below, tested in CPU i5-12600KF:
- Null: `{ "#nullx": null }`
- Array: `{ "#arrayx": [1, "string", true, null] }`
- Object: `{ "#objectx": {{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", null}} }`
- String: `{ "#stringx": "Hello World!" }`
- Number: `{ "#numberx": "31415" }`
- Boolean: `{ "#boolx": true }`
```
[===============================================================]
[--------------- Run tiny_json performance test ----------------]
[----------------------- Write benchmark -----------------------]
|     Null  |  Array  |  Object |  String | Number  | Boolean   |
[---------------- Scale of types in one object -----------------]
|    10000  |  10000  |  10000  |  10000  |  10000  |  10000    |
[----------------------- Write JSON (ms) -----------------------]
|                    str_size: 2373342, 47ms                    |
[===============================================================]

[===============================================================]
[--------------- Run tiny_json performance test ----------------]
[----------------------- Read benchmark ------------------------]
|     Null  |  Array  |  Object |  String | Number  | Boolean   |
[---------------- Scale of types in one object -----------------]
|    1000   |  1000   |  1000   |  1000   |  1000   |  1000     |
[------------------------ Read JSON (ms) -----------------------]
|                    str_size: 231342, 125ms                    |
[===============================================================]

[===============================================================]
[--------------- Run tiny_json performance test ----------------]
[---------------------- Class benchmark ------------------------]
|     Null  |  Array  |  Object |  String | Number  | Boolean   |
[---------------- Scale of types in one object -----------------]
|    100000 |  100000 |  100000 |  100000 |  100000 |  100000   |
[--------------------- Construct time (ms) ---------------------]
|    15     |  110    |  203    |  47     |  31     |  47       |
[--------------------- Destruct time (ms) ----------------------]
|    31     |  47     |  62     |  47     |  32     |  31       |
[===============================================================]

[===============================================================]
[--------------- Run tiny_json performance test ----------------]
|     Null  |  Array  |  Object |  String | Number  | Boolean   |
[---------------- Scale of types in one object -----------------]
|    10     |  20     |  50     |  50     |  20     |  10       |
[---------------------------- Loops ----------------------------]
|                             10000                             |
[--------------------- Construct time (ms) ---------------------]
[--------------------- Includes write JSON ---------------------]
|                             3640                              |
[===============================================================]
```

### UML Diagram
![uml](uml.jpg)

### TODO
- [x] JSON5 fully supported
- [x] Test frame
- [x] Normal pointer version
- [x] Optimize class performance
- [ ] Optimize read performance
