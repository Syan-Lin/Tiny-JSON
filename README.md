# Tiny-JSON
![](https://img.shields.io/badge/c%2B%2B-11-blue) ![](https://img.shields.io/badge/release-v1.2-blue) ![](https://img.shields.io/badge/coverage-100%25-green) ![](https://img.shields.io/badge/license-mit-blue)

### 介绍
Tiny-JSON 是一个用 C++11 实现的轻量化 JSON 解析和生成器（支持 JSON5），编码风格参考了 C++ 谷歌命名规范，注释丰富，无第三方库依赖。

[English version](https://github.com/Syan-Lin/Tiny-JSON/blob/main/README-en.md)

### Features
- 读取 JSON 文件，解析成 C++ 对象
- 使用 C++ 对象生成 JSON 文件
- 较高的代码可读性
- 支持 JSON5
- 极简的用法

### 如何使用
引入 `tiny_json.h` 需要编译 `tiny_json.cpp` 和 `tiny_json_frame.cpp` 两个文件

### 示例
编译器版本：`gcc version 8.1.0 (x86_64-posix-seh-rev0)`

```cpp
#include "tiny_json.h"
using namespace tiny_json;

// 1. 读取 JSON 并转化为 C++ 对象
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

// 2. 使用 C++ 对象生成 JSON 文件
Object obj2 = {
    {"number", 5.2},
    {"string", "Hello World!"},
    {"bool", false},
    {"array", Array({1, false, "abc"})}
};
// 生成 JSON 字符串
string json2 = parse(obj2);
```

>更多示例请看 `tiny_json_example.cpp` 和 `tiny_json_test.cpp`

### 测试
#### 用例测试
测试框架中简单定义了一些断言，用于判断布尔值、数值和字符串，当断言为假时，会打印相应的代码信息，例如：
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
通过 `tiny_json_test::Test::show_details_ = true` 可以展示详细断言信息：
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
#### 性能测试
定义在 `tiny_json_performance.h` 中，可以单独设置各类型的规模，以查看其实际性能表现

测试样例如下，测试 CPU 型号为 i5-12600KF：
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
|                    str_size: 231342, 63ms                     |
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

### UML 图
![uml](uml.jpg)

### TODO
- [x] JSON5 完全支持
- [x] 测试框架
- [x] 普通指针版本
- [x] 类性能优化
- [x] 读取性能优化
- [ ] Linux 支持
- [ ] 多线程性能测试
