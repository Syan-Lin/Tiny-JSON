#include <iostream>
#include <string>
#include <assert.h>
#include <regex>
#include <Windows.h>
#include "../tiny_json.h"
#include "tiny_json_performance.h"

using namespace std;
using namespace tiny_json;
using namespace tiny_json_test;
using namespace tiny_json_performance;

// 测试相关宏
std::string info;
#define EXPECT_TRUE(var, condition)             \
    info = __FILE__;                            \
    info += " at line: " + to_string(__LINE__); \
    var.ExpectTrue(condition, info)
#define EXPECT_FALSE(var, condition)            \
    info = __FILE__;                            \
    info += " at line: " + to_string(__LINE__); \
    var.ExpectFalse(condition, info)
#define EXPECT_INT(var, target, number)         \
    info = __FILE__;                            \
    info += " at line: " + to_string(__LINE__); \
    var.ExpectInt(target, number, info)
#define EXPECT_DOUBLE(var, target, number)      \
    info = __FILE__;                            \
    info += " at line: " + to_string(__LINE__); \
    var.ExpectDouble(target, number, info)
#define EXPECT_STRING(var, target, str)         \
    info = __FILE__;                            \
    info += " at line: " + to_string(__LINE__); \
    var.ExpectString(target, str, info)

// 外部函数测试
void FuncTest(){
    Test test("FuncTest");

    string json = "{"
        "\"integer\": 1,"
        "\"float\": 1.1,"
        "\"string\": \"Hello \\\"World!'\","
        "\"bool\": true,"
        "\"array\": ["
        "1,"
        "\"string\","
        "false,"
        "[\"peter\", \"bob\"],"
        "{\"name\": \"[Anna]\", \"age\": 16}"
        "],"
        "\"object\":{"
        "\"name\": \"Anna\","
        "\"age\": 16"
        "}"
        "}";
    Object o4 = parse(json);
    EXPECT_INT(test, 6, o4.size());
    EXPECT_STRING(test, "{\"array\": [1, \"string\", false, [\"peter\", \"bob\"], "
    "{\"age\": 16, \"name\": \"[Anna]\"}], \"bool\": true, \"float\": 1.1, \"integer\": 1, "
    "\"object\": {\"age\": 16, \"name\": \"Anna\"}, \"string\": \"Hello \\\"World!'\"}", o4.parse());

    // 环测试，由于是复制操作，所以不会产生环
    Object circle;
    Object temp;
    temp.add("obj", Value(circle));
    circle.add("str", Value("abc"));
    circle.add("arrar", Value(Array({1, 2, circle})));
    circle.add("obj", Value(circle));
    circle.add("str2", Value("ABC"));
    EXPECT_STRING(test, "{\"arrar\": [1, 2, {\"str\": \"abc\"}], \"obj\": {\"arrar\": [1, 2, {\"str\": \"abc\"}], \"str\": \"abc\"}, \"str\": \"abc\", \"str2\": \"ABC\"}", circle.parse());

    // 读写文件测试
    Object o1;
    for(int i = 0; i < 11; i++){
        string path = "Tests/test" + to_string(i+1) + ".json";
        o1 = readFile(path);
        string file = "Tests/test_out" + to_string(i+1) + ".json";
        writeFile(file, parse(o1));
    }
}

// Object 类测试，覆盖率 100%
void ObjectTest(){
    Test test("ObjectTest");

    // 初始化
    Object o1;
    o1.initFromJSON("{\"number\": 1.1}");   // 字符串初始化
    Object o2(o1);                          // Object(Object&)
    Object o3(move(Object()));              // Object(Object&&)
    Object o3_ = {
        {"key", 1432},
        {"array", Array({1, 2, 3})},
        {"obj", {
            {"inside", "obj"},
            {"hh", true}
        }}
    };
    EXPECT_INT(test, 1, o1.size());
    EXPECT_STRING(test, "{\"number\": 1.1}", o1.parse());
    EXPECT_INT(test, 1, o2.size());
    EXPECT_STRING(test, "{\"number\": 1.1}", o2.parse());
    EXPECT_INT(test, 0, o3.size());
    EXPECT_STRING(test, "{}", o3.parse());
    EXPECT_INT(test, 3, o3_.size());
    EXPECT_STRING(test, R"({"array": [1, 2, 3], "key": 1432, "obj": {"hh": true, "inside": "obj"}})", o3_.parse());

    // 赋值
    o3 = o1;        // operator=(Object&)
    EXPECT_INT(test, 1, o3.size());
    EXPECT_STRING(test, "{\"number\": 1.1}", o3.parse());
    o3 = Object();  // operator=(Object&&)
    EXPECT_INT(test, 0, o3.size());
    EXPECT_STRING(test, "{}", o3.parse());
    o1["number"] = 5;
    o3_["array"][2] = o1["number"];
    o3["test"] = o3_["obj"];
    EXPECT_INT(test, 1, o1.size());
    EXPECT_STRING(test, "{\"number\": 5}", o1.parse());
    EXPECT_INT(test, 3, o3_.size());
    EXPECT_STRING(test, R"({"array": [1, 2, 5], "key": 1432, "obj": {"hh": true, "inside": "obj"}})", o3_.parse());
    EXPECT_INT(test, 1, o3.size());
    EXPECT_STRING(test, R"({"test": {"hh": true, "inside": "obj"}})", o3.parse());

    // 功能测试
    o3.clear();
    o3.add("bool", false);
    o3.add("string", "abc");
    o3["number"] = 5.2;
    if(o3.has("number")){
        double v = o3["number"].get<double>();
        EXPECT_DOUBLE(test, 5.2, v);
    }
    o3.remove("number");
    EXPECT_STRING(test, "{\"bool\": false, \"string\": \"abc\"}", o3.parse());
    o3.clear();
    EXPECT_STRING(test, "{}", o3.parse());

    string json = "{"
        "\"integer\": 1,"
        "\"float\": 1.1,"
        "\"string\": \"Hello \\\"World!'\","
        "\"bool\": true,"
        "\"array\": ["
        "1,"
        "\"string\","
        "false,"
        "[\"peter\", \"bob\"],"
        "{\"name\": \"[Anna]\", \"age\": 16}"
        "],"
        "\"object\":{"
        "\"name\": \"Anna\","
        "\"age\": 16"
        "}"
        "}";
    Object o4;
    o4.initFromJSON(json);
    EXPECT_INT(test, 6, o4.size());
    EXPECT_STRING(test, "{\"array\": [1, \"string\", false, [\"peter\", \"bob\"], "
    "{\"age\": 16, \"name\": \"[Anna]\"}], \"bool\": true, \"float\": 1.1, \"integer\": 1, "
    "\"object\": {\"age\": 16, \"name\": \"Anna\"}, \"string\": \"Hello \\\"World!'\"}", o4.parse());

    // 错误测试
    // o4.remove("hhh");
    // o4.add("integer", 6.6);         // key 已存在，覆盖
    // o4["abcdfg"].get<string>();       // 严重错误，行为未定义
}

// Array 类测试，覆盖率 100%
void ArrayTest(){
    Test test("ArrayTest");

    // 初始化
    Array a1;
    Array a2({3.14, false, Value(), "abc"});                // Array(initializer_list)
    Array a3(a2);                                           // Array(Array&)
    Array a4(move(Array({1, 2, 3})));                       // Array(Array&&)
    Array a5, a5_;                                          // 字符串初始化
    a5.initFromJSON("[3.14,false,null,\"qwe\"]");
    a5_.initFromJSON("[3.14]");
    EXPECT_INT(test, 0, a1.size());
    EXPECT_STRING(test, "[]", a1.parse());
    EXPECT_INT(test, 4, a2.size());
    EXPECT_STRING(test, "[3.14, false, null, \"abc\"]", a2.parse());
    EXPECT_INT(test, 4, a3.size());
    EXPECT_STRING(test, "[3.14, false, null, \"abc\"]", a3.parse());
    EXPECT_INT(test, 3, a4.size());
    EXPECT_STRING(test, "[1, 2, 3]", a4.parse());
    EXPECT_INT(test, 4, a5.size());
    EXPECT_STRING(test, "[3.14, false, null, \"qwe\"]", a5.parse());
    EXPECT_INT(test, 1, a5_.size());
    EXPECT_STRING(test, "[3.14]", a5_.parse());

    // 赋值
    Array a6, a7;
    a6 = a2;                    // operator=(Array&)
    a7 = Array({1, 2, 3});      // operator=(Array&&)
    EXPECT_INT(test, 4, a6.size());
    EXPECT_STRING(test, "[3.14, false, null, \"abc\"]", a6.parse());
    EXPECT_INT(test, 3, a7.size());
    EXPECT_STRING(test, "[1, 2, 3]", a7.parse());

    // 功能测试
    a7[0] = 6.28;
    a7[1] = true;
    a7.append(Value(5));
    a7.append("hello");
    a7.append(false);
    a7.append(3.14);
    a7.append(3);
    a7.append(nullptr);
    EXPECT_INT(test, 9, a7.size());
    EXPECT_STRING(test, "[6.28, true, 3, 5, \"hello\", false, 3.14, 3, null]", a7.parse());
    a7.clear();
    EXPECT_INT(test, 0, a7.size());
    EXPECT_STRING(test, "[]", a7.parse());
    a7.add(0, nullptr);
    a7.add(1, Value(false));
    a7.add(2, "abc");
    a7.add(2, true);
    a7.add(4, 4);
    a7.add(5, 6.3);
    EXPECT_INT(test, 6, a7.size());
    EXPECT_STRING(test, "[null, false, true, \"abc\", 4, 6.3]", a7.parse());
    a7[1] = nullptr;
    a7[0] = 1;
    a7[2] = 1.1;
    a7[3] = true;
    a7[4] = "hello";
    a7[5] = Value();
    EXPECT_INT(test, 6, a7.size());
    EXPECT_STRING(test, "[1, null, 1.1, true, \"hello\", null]", a7.parse());
    a7.del(0);
    a7.del(1);
    a7.pop();
    EXPECT_INT(test, 3, a7.size());
    EXPECT_STRING(test, "[null, true, \"hello\"]", a7.parse());

    // 嵌套测试
    Array t;
    t.initFromJSON("[1, {\"num\": 2}, 3, [1, [\"abc\", {\"a\": \"ggg\"}], 3]]");
    EXPECT_INT(test, 4, t.size());
    EXPECT_STRING(test, "[1, {\"num\": 2}, 3, [1, [\"abc\", {\"a\": \"ggg\"}], 3]]", t.parse());

    // 错误测试
    // Array e;
    // 数组越界
    // e.pop();
    // e.del(1);
    // e.set(2, 4);
    // e[5] = 6;
    // e.initFromJSON("错误字符串，无法初始化");
    // e.initFromJSON("['asd\", 2, null]");        // 引号不成对
    // e.initFromJSON("{\"asd\", 2, null]");       // 括号错误
    // e.initFromJSON("[\"asd\", [}, 2, null]");   // 内部括号错误
    // e.initFromJSON("");
}

// Value 类测试，覆盖率 100%
void ValueTest(){
    Test test("ValueTest");

    // 初始化
    Value v1;                       // 默认为 null
    Value v2(3.14);                 // Value(double)
    Value v2_(3);                   // Value(int)
    Value v3(true);                 // Value(bool)
    Value v4("abc");                // Value(char[])
    string str = "abc";
    Value v4_str(str);              // Value(string&)
    Value v5(Array({1, 2, 3, false, "123"}));           // Value(Array&&)
    Array a({1, 2, 3, false, "123"});
    Value v5_(a);                                       // Value(Array&)
    Object o;
    o.initFromJSON("{\"num\": 1}");
    Object temp;
    temp.initFromJSON("{\"num\": 2.2}");
    Value v6(o);                    // Value(Object&)
    Value v6_(move(temp));          // Value(Object&&)
    Value v7(v2);                   // Value(Value&) 等价于 Value v8 = v2
    Value v8(move(Value(true)));    // Value(Value&&) 等价于 Value v7 = Value(true)
    Value v9 = nullptr;
    EXPECT_TRUE(test, v1.getType() == Type::kNull);
    EXPECT_STRING(test, "null", v1.parse());
    EXPECT_TRUE(test, v2.getType() == Type::kNumber);
    EXPECT_STRING(test, "3.14", v2.parse());
    EXPECT_TRUE(test, v2_.getType() == Type::kNumber);
    EXPECT_STRING(test, "3", v2_.parse());
    EXPECT_TRUE(test, v3.getType() == Type::kBoolean);
    EXPECT_STRING(test, "true", v3.parse());
    EXPECT_TRUE(test, v4.getType() == Type::kString);
    EXPECT_STRING(test, "\"abc\"", v4.parse());
    EXPECT_TRUE(test, v4_str.getType() == Type::kString);
    EXPECT_STRING(test, "\"abc\"", v4_str.parse());
    EXPECT_TRUE(test, v5.getType() == Type::kArray);
    EXPECT_STRING(test, "[1, 2, 3, false, \"123\"]", v5.parse());
    EXPECT_TRUE(test, v5_.getType() == Type::kArray);
    EXPECT_STRING(test, "[1, 2, 3, false, \"123\"]", v5_.parse());
    EXPECT_TRUE(test, v6.getType() == Type::kObject);
    EXPECT_STRING(test, "{\"num\": 1}", v6.parse());
    EXPECT_TRUE(test, v6_.getType() == Type::kObject);
    EXPECT_STRING(test, "{\"num\": 2.2}", v6_.parse());
    EXPECT_TRUE(test, v7.getType() == Type::kNumber);
    EXPECT_STRING(test, "3.14", v7.parse());
    EXPECT_TRUE(test, v8.getType() == Type::kBoolean);
    EXPECT_STRING(test, "true", v8.parse());
    EXPECT_TRUE(test, v9.getType() == Type::kNull);
    EXPECT_STRING(test, "null", v9.parse());

    // 移动语义测试
    Value m1(move(Value(2.2)));
    Value m2(move(Value("2.2")));
    Value m3(move(Value()));
    Value m4(move(Value(Object())));
    Value m5(move(Value(Array({1, 2, 3}))));
    EXPECT_TRUE(test, m1.getType() == Type::kNumber);
    EXPECT_STRING(test, "2.2", m1.parse());
    EXPECT_TRUE(test, m2.getType() == Type::kString);
    EXPECT_STRING(test, "\"2.2\"", m2.parse());
    EXPECT_TRUE(test, m3.getType() == Type::kNull);
    EXPECT_STRING(test, "null", m3.parse());
    EXPECT_TRUE(test, m4.getType() == Type::kObject);
    EXPECT_STRING(test, "{}", m4.parse());
    EXPECT_TRUE(test, m5.getType() == Type::kArray);
    EXPECT_STRING(test, "[1, 2, 3]", m5.parse());

    // 字符串初始化
    Value s1, s2, s3, s4, s5, s6, s7;
    s1.initFromJSON("3.14");        // 初始化为 Number 类型
    s6.initFromJSON("0x123");       // 初始化为 Number 类型
    s2.initFromJSON("true");        // 初始化为 Boolean 类型
    s3.initFromJSON("null");        // 初始化为 Null 类型
    s4.initFromJSON("\"abc\"");     // 初始化为 String 类型
    s5.initFromJSON("[1, 2, 3]");   // 初始化为 Array 类型
    s7.initFromJSON("{\"num\": 2.2}");// 初始化为 Object 类型
    EXPECT_TRUE(test, s1.getType() == Type::kNumber);
    EXPECT_STRING(test, "3.14", s1.parse());
    EXPECT_TRUE(test, s2.getType() == Type::kBoolean);
    EXPECT_STRING(test, "true", s2.parse());
    EXPECT_TRUE(test, s3.getType() == Type::kNull);
    EXPECT_STRING(test, "null", s3.parse());
    EXPECT_TRUE(test, s4.getType() == Type::kString);
    EXPECT_STRING(test, "\"abc\"", s4.parse());
    EXPECT_TRUE(test, s5.getType() == Type::kArray);
    EXPECT_STRING(test, "[1, 2, 3]", s5.parse());
    EXPECT_TRUE(test, s6.getType() == Type::kNumber);
    EXPECT_STRING(test, "0x123", s6.parse());
    EXPECT_TRUE(test, s7.getType() == Type::kObject);
    EXPECT_STRING(test, "{\"num\": 2.2}", s7.parse());

    // 赋值
    s1 = 5.68;      // operator=(double)
    s6 = 5;         // operator=(int)
    s2 = false;     // operator=(bool)
    s3 = "abc";     // operator=(char[]) -> operator=(string&)
    s4 = v8;        // operator=(Value&)
    s5 = Value();   // operator=(Value&&)
    EXPECT_TRUE(test, s1.getType() == Type::kNumber);
    EXPECT_STRING(test, "5.68", s1.parse());
    EXPECT_TRUE(test, s2.getType() == Type::kBoolean);
    EXPECT_STRING(test, "false", s2.parse());
    EXPECT_TRUE(test, s3.getType() == Type::kString);
    EXPECT_STRING(test, "\"abc\"", s3.parse());
    EXPECT_TRUE(test, s4.getType() == Type::kBoolean);
    EXPECT_STRING(test, "true", s4.parse());
    EXPECT_TRUE(test, s5.getType() == Type::kNull);
    EXPECT_STRING(test, "null", s5.parse());
    EXPECT_TRUE(test, s6.getType() == Type::kNumber);
    EXPECT_STRING(test, "0x5", s6.parse());

    Value t1, t2, t3, t4, t5, t6;
    t1 = Object({{"hello", 123}});
    t2 = Array({1, 2, 3});
    t3 = 1;
    t4 = nullptr;
    t5 = true;
    t6 = "10";
    EXPECT_TRUE(test, t1.getType() == Type::kObject);
    EXPECT_STRING(test, "{\"hello\": 123}", t1.parse());
    EXPECT_TRUE(test, t2.getType() == Type::kArray);
    EXPECT_STRING(test, "[1, 2, 3]", t2.parse());
    EXPECT_TRUE(test, t3.getType() == Type::kNumber);
    EXPECT_STRING(test, "1", t3.parse());
    EXPECT_TRUE(test, t4.getType() == Type::kNull);
    EXPECT_STRING(test, "null", t4.parse());
    EXPECT_TRUE(test, t5.getType() == Type::kBoolean);
    EXPECT_STRING(test, "true", t5.parse());
    EXPECT_TRUE(test, t6.getType() == Type::kString);
    EXPECT_STRING(test, "\"10\"", t6.parse());

    // 功能测试
    EXPECT_STRING(test, "{\"hello\": 123}", t1.get<Object>().parse());
    EXPECT_INT(test, 123, t1["hello"].get<int>());
    EXPECT_STRING(test, "[1, 2, 3]", t2.get<Array>().parse());
    EXPECT_INT(test, 1, t2[0].get<int>());
    EXPECT_INT(test, 1, t3.get<int>());
    EXPECT_STRING(test, "null", t4.parse());
    EXPECT_TRUE(test, t5.get<bool>());
    EXPECT_STRING(test, "10", t6.get<string>());
    v2 = 6.15;
    v3 = false;
    v4 = "ABC";
    EXPECT_TRUE(test, v2.getType() == Type::kNumber);
    EXPECT_STRING(test, "6.15", v2.parse());
    EXPECT_TRUE(test, v3.getType() == Type::kBoolean);
    EXPECT_STRING(test, "false", v3.parse());
    EXPECT_TRUE(test, v4.getType() == Type::kString);
    EXPECT_STRING(test, "\"ABC\"", v4.parse());
    v2.reset();
    EXPECT_TRUE(test, v2.getType() == Type::kNull);
    EXPECT_STRING(test, "null", v2.parse());
    v1 = true;
    v2 = "abc";      // set(char[]) -> set(string&)
    v3 = 2;
    v4 = 3.5;
    EXPECT_TRUE(test, v2.getType() == Type::kString);
    EXPECT_STRING(test, "\"abc\"", v2.parse());
    EXPECT_TRUE(test, v3.getType() == Type::kNumber);
    EXPECT_STRING(test, "2", v3.parse());
    EXPECT_TRUE(test, v4.getType() == Type::kNumber);
    EXPECT_STRING(test, "3.5", v4.parse());
    EXPECT_TRUE(test, v1.getType() == Type::kBoolean);
    EXPECT_STRING(test, "true", v1.parse());
    v7 = Object();
    v8 = Array({1, 2 ,3});
    EXPECT_TRUE(test, v7.getType() == Type::kObject);
    EXPECT_STRING(test, "{}", v7.parse());
    EXPECT_TRUE(test, v8.getType() == Type::kArray);
    EXPECT_STRING(test, "[1, 2, 3]", v8.parse());

    // 错误测试
    // static_cast<Boolean&>(v2.get()).set(false);     // 类型错误，行为未定义，在使用之前请检查类型!
    // Value e;
    // e.initFromJSON("qwe");      // 无法转化为任何类型
    // e.initFromJSON("");         // 无法转化为任何类型
}

// 正则表达式测试
void RegExTest(){
    Test test("RegExTest");

    using namespace reg_ex;
    // Number 对象
    // 实数正则式
    EXPECT_TRUE(test, regex_match("1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("1.1", kPatternNumber));
    EXPECT_TRUE(test, regex_match(".1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("0.1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("1.1e2", kPatternNumber));
    EXPECT_TRUE(test, regex_match("1.1e+2", kPatternNumber));
    EXPECT_TRUE(test, regex_match("1.1e-2", kPatternNumber));
    EXPECT_TRUE(test, regex_match("+1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("+1.1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("+.1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("+0.1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("+1.1e2", kPatternNumber));
    EXPECT_TRUE(test, regex_match("+1.1e+2", kPatternNumber));
    EXPECT_TRUE(test, regex_match("+1.1e-2", kPatternNumber));
    EXPECT_TRUE(test, regex_match("-1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("-1.1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("-.1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("-0.1", kPatternNumber));
    EXPECT_TRUE(test, regex_match("-1.1e2", kPatternNumber));
    EXPECT_TRUE(test, regex_match("-1.1e+2", kPatternNumber));
    EXPECT_TRUE(test, regex_match("-1.1e-2", kPatternNumber));
    // 十六进制正则式
    EXPECT_TRUE(test, regex_match("0xabcdef123", kPatternHex));
    EXPECT_TRUE(test, regex_match("0Xabcdef123", kPatternHex));
    EXPECT_TRUE(test, regex_match("0xABCDEF123", kPatternHex));
    EXPECT_FALSE(test, regex_match("123456789", kPatternHex));
    EXPECT_TRUE(test, regex_match("abcdef123", kPatternHex));
    EXPECT_TRUE(test, regex_match("ABCDEF123", kPatternHex));

    // String 对象 '...' 模式或 "..." 模式
    EXPECT_TRUE(test, regex_match("\"0xabcdef123\"", kPatternString));
    EXPECT_TRUE(test, regex_match("\"中文字符test\n\t\\\"", kPatternString));
    EXPECT_TRUE(test, regex_match("\"中文字符测试\"", kPatternString));
    EXPECT_TRUE(test, regex_match("\"This is a test \t !!!\"", kPatternString));
    EXPECT_TRUE(test, regex_match("\"\u235afasd\"", kPatternString));
    EXPECT_TRUE(test, regex_match("\'0xabcdef123\'", kPatternString));
    EXPECT_TRUE(test, regex_match("\'中文字符test\n\t\\\'", kPatternString));
    EXPECT_TRUE(test, regex_match("\'中文字符测试\'", kPatternString));
    EXPECT_TRUE(test, regex_match("\'This is a test \t !!!\'", kPatternString));
    EXPECT_TRUE(test, regex_match("\'\u235aunicode\'", kPatternString));

    // Boolean 对象
    EXPECT_TRUE(test, regex_match("true", kPatternBool));
    EXPECT_TRUE(test, regex_match("false", kPatternBool));
    EXPECT_FALSE(test, regex_match("True", kPatternBool));
    EXPECT_FALSE(test, regex_match("False", kPatternBool));

    // Null 对象
    EXPECT_TRUE(test, regex_match("null", kPatternNull));
    EXPECT_FALSE(test, regex_match("Null", kPatternNull));

    // Array 对象 [...] 模式
    EXPECT_TRUE(test, regex_match("[1,2,4521,\"Test\nTest\",]", kPatternArray));

    // Object 对象 ..:.. 模式
    EXPECT_TRUE(test, regex_match("key: \"value\"", kPatternObj));
    EXPECT_TRUE(test, regex_match("\"key\": \'value\'", kPatternObj));
    EXPECT_TRUE(test, regex_match("key123_=+: \"value123_=\"", kPatternObj));
    EXPECT_TRUE(test, regex_match("\"key123_=+\": 1.234e-5", kPatternObj));
}

// Number 类测试，覆盖率 100%
void NumberTest(){
    Test test("NumberTest");

    // 初始化
    double d = 3.14;
    double i = 3;
    Number n1;                      // 默认为十进制 0
    Number n2(d);                   // Number(double)
    Number n2_(i);                  // Number(double) 自动转整数
    Number n4, n5;
    Number n6(3);                   // Number(int)
    n4.initFromJSON("0xabc123");    // 字符串初始化
    n5.initFromJSON("abc123");      // 字符串初始化
    EXPECT_TRUE(test, n1.getType() == NumberType::kDefault);
    EXPECT_DOUBLE(test, 0, n1.getDouble());
    EXPECT_STRING(test, "0", n1.parse());
    EXPECT_TRUE(test, n2.getType() == NumberType::kDefault);
    EXPECT_DOUBLE(test, 3.14, n2.getDouble());
    EXPECT_STRING(test, "3.14", n2.parse());
    EXPECT_TRUE(test, n2_.getType() == NumberType::kInteger);
    EXPECT_INT(test, 3, n2_.getInt());
    EXPECT_STRING(test, "3", n2_.parse());
    EXPECT_TRUE(test, n4.getType() == NumberType::kHex);
    EXPECT_INT(test, 11256099, n4.getInt());
    EXPECT_STRING(test, "0xabc123", n4.parse());
    EXPECT_TRUE(test, n5.getType() == NumberType::kHex);
    EXPECT_INT(test, 11256099, n5.getInt());
    EXPECT_STRING(test, "0xabc123", n5.parse());
    EXPECT_TRUE(test, n6.getType() == NumberType::kInteger);
    EXPECT_INT(test, 3, n6.getInt());
    EXPECT_STRING(test, "3", n6.parse());

    // 赋值
    Number n7, n8;
    n7 = 3.14;                     // operator=(double)
    n8 = 3;                        // operator=(int)
    EXPECT_TRUE(test, n7.getType() == NumberType::kDefault);
    EXPECT_DOUBLE(test, 3.14, n7.getDouble());
    EXPECT_STRING(test, "3.14", n7.parse());
    EXPECT_TRUE(test, n8.getType() == NumberType::kInteger);
    EXPECT_INT(test, 3, n8.getInt());
    EXPECT_STRING(test, "3", n8.parse());

    // 功能测试
    n1 = 3.14159245622134131;
    EXPECT_TRUE(test, n1.getType() == NumberType::kDefault);
    EXPECT_DOUBLE(test, 3.14159245622134131, n1.getDouble());
    EXPECT_STRING(test, "3.14159", n1.parse());
    n1.parseSetting(NumberType::kFloat, 18);
    EXPECT_TRUE(test, n1.getType() == NumberType::kFloat);
    EXPECT_DOUBLE(test, 3.14159245622134131, n1.getDouble());
    EXPECT_STRING(test, "3.14159245622134131", n1.parse());
    n2 = 1234567890;
    EXPECT_TRUE(test, n2.getType() == NumberType::kInteger);
    EXPECT_INT(test, 1234567890, n2.getInt());
    EXPECT_STRING(test, "1234567890", n2.parse());
    n4.parseSetting(NumberType::kInteger);
    EXPECT_TRUE(test, n4.getType() == NumberType::kInteger);
    EXPECT_INT(test, 11256099, n4.getInt());
    EXPECT_STRING(test, "11256099", n4.parse());
    n1 = 5;      // kFloat -> kInteger
    n2 = 5;      // kDefault -> kInteger
    n5 = 3.14;   // kHex -> kDefault
    n4 = 3.14;   // kInteger -> kDefault
    EXPECT_TRUE(test, n1.getType() == NumberType::kInteger);
    EXPECT_INT(test, 5, n1.getInt());
    EXPECT_STRING(test, "5", n1.parse());
    EXPECT_TRUE(test, n2.getType() == NumberType::kInteger);
    EXPECT_INT(test, 5, n2.getInt());
    EXPECT_STRING(test, "5", n2.parse());
    EXPECT_TRUE(test, n5.getType() == NumberType::kDefault);
    EXPECT_DOUBLE(test, 3.14, n5.getDouble());
    EXPECT_STRING(test, "3.14", n5.parse());
    EXPECT_TRUE(test, n4.getType() == NumberType::kDefault);
    EXPECT_DOUBLE(test, 3.14, n4.getDouble());
    EXPECT_STRING(test, "3.14", n4.parse());

    // 数字格式
    {
        Number s1, s2, s3, s4, s5, s6;
        s1.initFromJSON("1");
        s2.initFromJSON("1.1");
        s3.initFromJSON(".1");
        s4.initFromJSON("1.");
        s5.initFromJSON("0xa");
        s6.initFromJSON("1.1e2");
        EXPECT_TRUE(test, s1.getType() == NumberType::kDefault);
        EXPECT_INT(test, 1, s1.getInt());
        EXPECT_STRING(test, "1", s1.parse());
        EXPECT_TRUE(test, s2.getType() == NumberType::kDefault);
        EXPECT_DOUBLE(test, 1.1, s2.getDouble());
        EXPECT_STRING(test, "1.1", s2.parse());
        EXPECT_TRUE(test, s3.getType() == NumberType::kDefault);
        EXPECT_DOUBLE(test, 0.1, s3.getDouble());
        EXPECT_STRING(test, "0.1", s3.parse());
        EXPECT_TRUE(test, s4.getType() == NumberType::kDefault);
        EXPECT_INT(test, 1, s4.getInt());
        EXPECT_STRING(test, "1", s4.parse());
        EXPECT_TRUE(test, s5.getType() == NumberType::kHex);
        EXPECT_INT(test, 10, s5.getInt());
        EXPECT_STRING(test, "0xa", s5.parse());
        EXPECT_TRUE(test, s6.getType() == NumberType::kDefault);
        EXPECT_INT(test, 110, s6.getInt());
        EXPECT_STRING(test, "110", s6.parse());
    }
    {
        Number s1, s2, s3, s4, s5, s6;
        s1.initFromJSON("-1");
        s2.initFromJSON("-1.1");
        s3.initFromJSON("-.1");
        s4.initFromJSON("-1.");
        s5.initFromJSON("-0xa");
        s6.initFromJSON("-1.1e2");
        EXPECT_TRUE(test, s1.getType() == NumberType::kDefault);
        EXPECT_INT(test, -1, s1.getInt());
        EXPECT_STRING(test, "-1", s1.parse());
        EXPECT_TRUE(test, s2.getType() == NumberType::kDefault);
        EXPECT_DOUBLE(test, -1.1, s2.getDouble());
        EXPECT_STRING(test, "-1.1", s2.parse());
        EXPECT_TRUE(test, s3.getType() == NumberType::kDefault);
        EXPECT_DOUBLE(test, -0.1, s3.getDouble());
        EXPECT_STRING(test, "-0.1", s3.parse());
        EXPECT_TRUE(test, s4.getType() == NumberType::kDefault);
        EXPECT_INT(test, -1, s4.getInt());
        EXPECT_STRING(test, "-1", s4.parse());
        EXPECT_TRUE(test, s5.getType() == NumberType::kDefault);
        EXPECT_INT(test, -10, s5.getInt());
        EXPECT_STRING(test, "-10", s5.parse());
        EXPECT_TRUE(test, s6.getType() == NumberType::kDefault);
        EXPECT_INT(test, -110, s6.getInt());
        EXPECT_STRING(test, "-110", s6.parse());
    }

    // 错误测试
    // Number n9;
    // n9.initFromJSON("a.56");                    // 十六进制不支持小数，丢失精度
    // assert(n9.getInt() == 10 && n9.parse() == "0xa" && n9.getType() == NumberType::kHex);
    // Number n10;
    // n10.initFromJSON("qabc5q.56");               // 非法的字符串
    // n4.parseSetting(NumberType::kInteger);      // 浮点数以整数输出会丢失精度
    // assert(n4.getInt() == 3 && n4.parse() == "3");
    // n5.parseSetting(NumberType::kHex);          // 浮点数以十六进制输出会丢失精度
    // assert(n5.getInt() == 3 && n5.parse() == "0x3");
    // n5.initFromJSON("");
}

// String 类测试，覆盖率 100%
void StringTest(){
    Test test("StringTest");

    // 初始化
    string str = "test\nnext\tline\n";
    String s1(str);                             // String(string&)
    String s2(s1);                              // String(String&)
    String s3("a String");                      // String(char[]) -> String(string&&)
    String s4(string("a string"));              // String(string&&)
    String s5;                                  // String()
    String s6(move(String("move String")));     // String(String&&)
                                                // 这里实际上会经过 String(char[]) -> String(string&&) -> String(String&&)
                                                // 不过前两步是构造 String("move String") 这个右值对象
    String s7;                                  // 字符串初始化
    s7.initFromJSON(R"("initFromJSON")");
    EXPECT_STRING(test, "test\nnext\tline\n", s1.get());
    EXPECT_STRING(test, "\"test\\nnext\\tline\\n\"", s1.parse());
    EXPECT_STRING(test, "test\nnext\tline\n", s2.get());
    EXPECT_STRING(test, "\"test\\nnext\\tline\\n\"", s2.parse());
    EXPECT_STRING(test, "a String", s3.get());
    EXPECT_STRING(test, "\"a String\"", s3.parse());
    EXPECT_STRING(test, "a string", s4.get());
    EXPECT_STRING(test, "\"a string\"", s4.parse());
    EXPECT_STRING(test, "", s5.get());
    EXPECT_STRING(test, "\"\"", s5.parse());
    EXPECT_STRING(test, "move String", s6.get());
    EXPECT_STRING(test, "\"move String\"", s6.parse());
    EXPECT_STRING(test, "initFromJSON", s7.get());
    EXPECT_STRING(test, "\"initFromJSON\"", s7.parse());

    // 赋值
    String s8, s10, s11;
    s8 = str;                                     // operator=(string&)
    s10 = string("test\nnext\tline\n");           // operator=(string&&)
    s11 = "test\nnext\tline\n";                   // operator=(char[])
    EXPECT_STRING(test, "test\nnext\tline\n", s8.get());
    EXPECT_STRING(test, "\"test\\nnext\\tline\\n\"", s8.parse());
    EXPECT_STRING(test, "test\nnext\tline\n", s10.get());
    EXPECT_STRING(test, "\"test\\nnext\\tline\\n\"", s10.parse());
    EXPECT_STRING(test, "test\nnext\tline\n", s11.get());
    EXPECT_STRING(test, "\"test\\nnext\\tline\\n\"", s11.parse());

    // 功能测试
    s1 = "this is a message";
    EXPECT_STRING(test, "this is a message", s1.get());
    EXPECT_STRING(test, "\"this is a message\"", s1.parse());
    s1 = string("abc");
    EXPECT_STRING(test, "abc", s1.get());
    EXPECT_STRING(test, "\"abc\"", s1.parse());
    s1 = "";
    EXPECT_STRING(test, "", s1.get());
    EXPECT_STRING(test, "\"\"", s1.parse());

    // 转义字符测试
    String c;
    c.initFromJSON("\"\\\"\\b\\f\\t\\n\\r\\\\\\u002E\"");
    EXPECT_STRING(test, "\"\b\f\t\n\r\\0", c.get());
    EXPECT_STRING(test, "\"\\\"\\b\\f\\t\\n\\r\\\\0\"", c.parse());

    // 错误测试
    // String e;
    // e.initFromJSON("\"\"\"");     // '"' 未转义
    // e.initFromJSON("\"");         // 长度非法，至少需要两个引号
    // e.initFromJSON("\"\\p\"");    // \p 非法转义字符
    // e.initFromJSON("\"\\u1\"");   // \u1 Unicode 非法
    // e.initFromJSON("");           // 长度非法，至少需要两个引号
    // assert(e.get() == "");
    // assert(e.getJSON() == "\"\"");
    // assert(e.parse() == "\"\"");
    // assert(e.getJSON() == "\"\"");
}

// String JSON5 测试
// 在 JSON5 下，不支持单双引号混合使用，请一律使用单引号
void StringJSON5Test(){
    Test test("StringJSON5Test");

    String s1, s2, s3;
    s1.initFromJSON("'Hello World!'");          // 单引号测试
    s2.initFromJSON("'Hello \"World\"!'");      // 单引号测试
    s3.initFromJSON("'Hello \n\t\tWorld!'");    // 换行测试
    EXPECT_STRING(test, "Hello World!", s1.get());
    EXPECT_STRING(test, "'Hello World!'", s1.parse());
    EXPECT_STRING(test, "Hello \"World\"!", s2.get());
    EXPECT_STRING(test, "'Hello \"World\"!'", s2.parse());
    EXPECT_STRING(test, "Hello World!", s3.get());
    EXPECT_STRING(test, "'Hello World!'", s3.parse());

    // 错误测试
    // s1.initFromJSON("'Hello 'World!''");    // JSON5 单引号需要转义
    // s1.initFromJSON("\"Hello 'World\"");    // JSON5 的双引号视为单引号
}

// Object JSON5 测试
void ObjectJSON5Test(){
    Test test("ObjectJSON5Test");
    Object o1;
    o1.initFromJSON("{number: 1.1, age: 12,}");   // 字符串初始化
    EXPECT_INT(test, 2, o1.size());
    EXPECT_STRING(test, "{age: 12, number: 1.1}", o1.parse());

    // 错误测试
    // o1.initFromJSON("{number: 1.1, age: 12,,}");    // 多一个逗号
}

// Array JSON5 测试
void ArrayJSON5Test(){
    Test test("ArrayJSON5Test");
    Array a1;
    a1.initFromJSON("['number', 1.1, false, \n \t ]");   // 字符串初始化
    EXPECT_INT(test, 3, a1.size());
    EXPECT_STRING(test, "['number', 1.1, false]", a1.parse());

    // 错误测试
    // a1.initFromJSON("['number', 1.1, false,,]");    // 多一个逗号
}

void AnnotationTest(){
    Test test("AnnotationTest");
    string str(
            "// This is an Annotation\n"
            "{\n"
            "/*\n"
            "\tThis is a cross Annotation\n"
            "\tThis is a cross Annotation\n"
            "*/\n"
            "\"bool\": false,\n"
            "\"info\": \"GWB //this shall not be removed\",  // This is an Annotation\n"
            "\"data\": 123 // This is an Annotation\n"
            "}"
    );
    // 只有在 parse 函数中才能去除注释
    Object obj = parse(str);
    // 或者手动调用
    removeAnnotation(str);
    Object obj2;
    obj2.initFromJSON(str);
    EXPECT_STRING(test, "{bool: false, data: 123, info: 'GWB //this shall not be removed'}", obj.parse());
    EXPECT_STRING(test, "{bool: false, data: 123, info: 'GWB //this shall not be removed'}", obj2.parse());
}

// JSON5 混合测试
void JSON5Test(){
    // 读写文件测试
    Object o1;
    for(int i = 0; i < 11; i++){
        string path = "Tests-JSON5/test" + to_string(i+1) + ".json5";
        o1 = readFile(path);
        string file = "Tests-JSON5/test_out" + to_string(i+1) + ".json5";
        writeFile(file, parse(o1));
    }
}

void PerformanceTest(){
    Performance pt;
    pt.setScale(10000);
    // pt.setScale(10, Type::kNull);
    // pt.setScale(20, Type::kNumber);
    // pt.setScale(50, Type::kObject);
    // pt.setScale(50, Type::kString);
    // pt.setScale(10, Type::kBoolean);
    // pt.setScale(20, Type::kArray);
    // pt.runLoop(1);
    pt.run();
}

int main(){
    tiny_json_test::Test::show_details_ = false;

    JSON5 = false;
    NumberTest();
    StringTest();
    ValueTest();
    ArrayTest();
    ObjectTest();
    RegExTest();
    // FuncTest();

    // JSON5 测试
    JSON5 = true;
    StringJSON5Test();
    ObjectJSON5Test();
    ArrayJSON5Test();
    AnnotationTest();
    // JSON5Test();

    // 性能测试
    JSON5 = false;
    PerformanceTest();

    return 0;
}
