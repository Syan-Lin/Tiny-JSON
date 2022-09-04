# Tiny-JSON

### 介绍
Tiny-JSON 是一个用 C++11 实现的轻量化 JSON 解析和生成器，编码风格参考了 C++ 谷歌命名规范，无第三方库依赖，只依赖于标准库。

### Features
- 读取 JSON 文件，解析成 C++ 对象
- 使用 C++ 对象生成 JSON 文件

### 示例
编译器版本：`gcc version 8.1.0 (x86_64-posix-seh-rev0)`

```cpp
// 1. 读取 JSON 并转化为 C++ 对象
string json = "{"
        "\"integer\": 1,"
        "\"string\": \"Hello World!\","
        "\"array\": ["
        "1,"
        "\"string\","
        "false"
        "]"
        "}";
Object obj;
obj.initFromJSON(json);

// 获取数据前，确定对象包含该键值，否则行为未定义
// 获取值之后需要转化成对应的数据格式
if(obj.has("integer")){
    Value& v = obj.get("integer");
    if(v.getType() == Type::kNumber){
        int i = static_cast<Number&>(v.get()).get();
    }
}
if(obj.has("string")){
    Value& v = obj.get("string");
    if(v.getType() == Type::kString){
        string str = static_cast<String&>(v.get()).get();
    }
}
if(obj.has("array")){
    Value& v = obj.get("array");
    if(v.getType() == Type::kArray){
        Array arr = static_cast<Array&>(v.get());
        int first = static_cast<Number&>(arr[0].get()).get();
        string second = static_cast<String&>(arr[1].get()).get();
        bool third = static_cast<Boolean&>(arr[2].get()).get();
    }
}

// 2. 使用 C++ 对象生成 JSON 文件
Object obj;
obj.add("number", 5.2);
obj.add("string", "Hello World!");
obj.add("bool", false);
obj.add("array", Array({1, false, "abc"}));

// 生成 JSON 字符串
string json = parse(obj);
```

>更多示例请看 `tiny_json_test.cpp`

### TODO
- [ ] JSON5 完全支持
- [ ] UTF-8
