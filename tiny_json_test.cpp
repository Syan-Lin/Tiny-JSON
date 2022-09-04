#include <iostream>
#include <string>
#include <assert.h>
#include <regex>
#include <Windows.h>
#include "tiny_json.h"

using namespace std;
using namespace tiny_json;

// 外部函数测试
void FuncTest(){
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
    assert(o4.size() == 6 && parse(o4, false) == "{\"array\": [1, \"string\", false, [\"peter\", \"bob\"], "
    "{\"age\": 16, \"name\": \"[Anna]\"}], \"bool\": true, \"float\": 1.1, \"integer\": 1, "
    "\"object\": {\"age\": 16, \"name\": \"Anna\"}, \"string\": \"Hello \\\"World!'\"}");

    // 环测试，由于是复制操作，所以不会产生环
    Object circle;
    Object temp;
    temp.add("obj", Value(circle));
    circle.add("str", Value("abc"));
    circle.add("arrar", Value(Array({1, 2, circle})));
    circle.add("obj", Value(circle));
    circle.add("str2", Value("ABC"));
    assert(circle.parse() == "{\"arrar\": [1, 2, {\"str\": \"abc\"}], \"obj\": {\"arrar\": [1, 2, {\"str\": \"abc\"}], \"str\": \"abc\"}, \"str\": \"abc\", \"str2\": \"ABC\"}");

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
    // 四种初始化
    Object o1;
    o1.initFromJSON("{\"number\": 1.1}");   // 字符串初始化
    Object o2(o1);                          // Object(Object&)
    Object o3(move(Object()));              // Object(Object&&)
    assert(o1.size() == 1 && o1.parse() == "{\"number\": 1.1}");
    assert(o2.size() == 1 && o2.parse() == "{\"number\": 1.1}");
    assert(o3.size() == 0 && o3.parse() == "{}");

    // 两种赋值
    o3 = o1;        // operator=(Object&)
    assert(o3.size() == 1 && o3.parse() == "{\"number\": 1.1}");
    o3 = Object();  // operator=(Object&&)
    assert(o3.size() == 0 && o3.parse() == "{}");

    // 功能测试
    o3.add("bool", false);
    o3.add("string", "abc");
    o3.set("number", 5.2);
    if(o3.has("number")){
        Value& v = o3.get("number");
        assert(v.parse() == "5.2");
    }
    o3.del("number");
    assert(o3.parse() == "{\"bool\": false, \"string\": \"abc\"}");
    o3.reset();
    assert(o3.parse() == "{}");

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
    assert(o4.size() == 6 && o4.parse() == "{\"array\": [1, \"string\", false, [\"peter\", \"bob\"], "
    "{\"age\": 16, \"name\": \"[Anna]\"}], \"bool\": true, \"float\": 1.1, \"integer\": 1, "
    "\"object\": {\"age\": 16, \"name\": \"Anna\"}, \"string\": \"Hello \\\"World!'\"}");

    // 错误测试
    o4.del("hhh");
    o4.add("integer", 6.6); // key 已存在，覆盖
    o4.get("abcdfg");       // 严重错误，行为未定义
}

// Array 类测试，覆盖率 100%
void ArrayTest(){
    // 五种初始化
    Array a1;
    Array a2({3.14, false, Value(), "abc"});                // Array(initializer_list)
    Array a3(a2);                                           // Array(Array&)
    Array a4(move(Array({1, 2, 3})));                       // Array(Array&&)
    Array a5, a5_;                                          // 字符串初始化
    a5.initFromJSON("[3.14,false,null,\"qwe\"]");
    a5_.initFromJSON("[3.14]");
    assert(a1.size() == 0 && a1.parse() == "[]");
    assert(a2.size() == 4 && a2.parse() == "[3.14, false, null, \"abc\"]");
    assert(a3.size() == 4 && a3.parse() == "[3.14, false, null, \"abc\"]");
    assert(a4.size() == 3 && a4.parse() == "[1, 2, 3]");
    assert(a5.size() == 4 && a5.parse() == "[3.14, false, null, \"qwe\"]");
    assert(a5_.size() == 1 && a5_.parse() == "[3.14]");

    // 两种赋值
    Array a6, a7;
    a6 = a2;                    // operator=(Array&)
    a7 = Array({1, 2, 3});      // operator=(Array&&)
    assert(a6.size() == 4 && a6.parse() == "[3.14, false, null, \"abc\"]");
    assert(a7.size() == 3 && a7.parse() == "[1, 2, 3]");

    // 功能测试
    a7[0] = 6.28;           // 等价于 a7[0].set(6.28)
    a7[1].set(true);        // 等价于 a7[1] = true
    a7.append(Value(5));    // append(Value&)
    a7.append("hello");     // append(char[]) -> append(string&)
    a7.append(false);       // append(bool)
    a7.append(3.14);        // append(double)
    a7.append(3);           // append(int)
    a7.append();            // append()
    assert(a7.size() == 9 && a7.parse() == "[6.28, true, 3, 5, \"hello\", false, 3.14, 3, null]");
    a7.reset();
    assert(a7.size() == 0 && a7.parse() == "[]");
    a7.add(0);                  // add(size_t)
    a7.add(1, Value(false));    // add(size_t, Value&)
    a7.add(2, "abc");           // add(size_t, char[]) -> add(size_t, string&)
    a7.add(2, true);            // add(size_t, bool)
    a7.add(4, 4);               // add(size_t, int)
    a7.add(5, 6.3);             // add(size_t, double)
    assert(a7.size() == 6 && a7.parse() == "[null, false, true, \"abc\", 4, 6.3]");
    a7.set(1);                  // set(size_t)
    a7.set(0, 1);               // set(size_t, int)
    a7.set(2, 1.1);             // set(size_t, double)
    a7.set(3, true);            // set(size_t, bool)
    a7.set(4, "hello");         // set(size_t, char[]) -> set(size_t, string&)
    a7.set(5, Value());         // set(size_t, Value&)
    assert(a7.size() == 6 && a7.parse() == "[1, null, 1.1, true, \"hello\", null]");
    a7.del(0);
    a7.del(1);
    a7.pop();
    assert(a7.size() == 3 && a7.parse() == "[null, true, \"hello\"]");

    // 嵌套测试
    Array t;
    t.initFromJSON("[1, {\"num\": 2}, 3, [1, [\"abc\", {\"a\": \"ggg\"}], 3]]");
    assert(t.size() == 4 && t.parse() == "[1, {\"num\": 2}, 3, [1, [\"abc\", {\"a\": \"ggg\"}], 3]]");

    // 错误测试
    Array e;
    // 数组越界
    // e.pop();
    // e.del(1);
    // e.set(2, 4);
    // e[5] = 6;
    e.initFromJSON("错误字符串，无法初始化");
    e.initFromJSON("['asd\", 2, null]");        // 引号不成对
    e.initFromJSON("{\"asd\", 2, null]");       // 括号错误
    e.initFromJSON("[\"asd\", [}, 2, null]");   // 内部括号错误
    e.initFromJSON("");
}

// Value 类测试，覆盖率 100%
void ValueTest(){
    // 十三种初始化
    Value v1;                       // 默认为 null
    Value v2(3.14);                 // Value(double)
    Value v2_(3);                   // Value(int)
    Value v3(true);                 // Value(bool)
    Value v4("abc");                // Value(char[]) -> Value(string&&)
    Value v4_(string("abc"));       // Value(string&&)
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
    Value v6_(move(temp));             // Value(Object&&)
    Value v7(v2);                   // Value(Value&) 等价于 Value v8 = v2
    Value v8(move(Value(true)));    // Value(Value&&) 等价于 Value v7 = Value(true)
    assert(v1.getType() == Type::kNull && v1.parse() == "null");
    assert(v2.getType() == Type::kNumber && v2.parse() == "3.14");
    assert(v2_.getType() == Type::kNumber && v2_.parse() == "3");
    assert(v3.getType() == Type::kBoolean && v3.parse() == "true");
    assert(v4.getType() == Type::kString && v4.parse() == "\"abc\"");
    assert(v4_.getType() == Type::kString && v4_.parse() == "\"abc\"");
    assert(v4_str.getType() == Type::kString && v4_str.parse() == "\"abc\"");
    assert(v5.getType() == Type::kArray && v5.parse() == "[1, 2, 3, false, \"123\"]");
    assert(v5_.getType() == Type::kArray && v5_.parse() == "[1, 2, 3, false, \"123\"]");
    assert(v6.getType() == Type::kObject && v6.parse() == "{\"num\": 1}");
    assert(v6_.getType() == Type::kObject && v6_.parse() == "{\"num\": 2.2}");
    assert(v7.getType() == Type::kNumber && v7.parse() == "3.14");
    assert(v8.getType() == Type::kBoolean && v8.parse() == "true");

    // 移动语义测试
    Value m1(move(Value(2.2)));
    Value m2(move(Value("2.2")));
    Value m3(move(Value()));
    Value m4(move(Value(Object())));
    Value m5(move(Value(Array({1, 2, 3}))));
    assert(m1.getType() == Type::kNumber && m1.parse() == "2.2");
    assert(m2.getType() == Type::kString && m2.parse() == "\"2.2\"");
    assert(m3.getType() == Type::kNull && m3.parse() == "null");
    assert(m4.getType() == Type::kObject && m4.parse() == "{}");
    assert(m5.getType() == Type::kArray && m5.parse() == "[1, 2, 3]");

    // 字符串初始化
    Value s1, s2, s3, s4, s5, s6, s7;
    s1.initFromJSON("3.14");        // 初始化为 Number 类型
    s6.initFromJSON("0x123");       // 初始化为 Number 类型
    s2.initFromJSON("true");        // 初始化为 Boolean 类型
    s3.initFromJSON("null");        // 初始化为 Null 类型
    s4.initFromJSON("\"abc\"");     // 初始化为 String 类型
    s5.initFromJSON("[1, 2, 3]");   // 初始化为 Array 类型
    s7.initFromJSON("{\"num\": 2.2}");// 初始化为 Object 类型
    assert(s1.getType() == Type::kNumber && s1.parse() == "3.14");
    assert(s2.getType() == Type::kBoolean && s2.parse() == "true");
    assert(s3.getType() == Type::kNull && s3.parse() == "null");
    assert(s4.getType() == Type::kString && s4.parse() == "\"abc\"");
    assert(s5.getType() == Type::kArray && s5.parse() == "[1, 2, 3]");
    assert(s6.getType() == Type::kNumber && s6.parse() == "0x123");
    assert(s7.getType() == Type::kObject && s7.parse() == "{\"num\": 2.2}");

    // 五种赋值
    s1 = 5.68;      // operator=(double)
    s6 = 5;         // operator=(int)
    s2 = false;     // operator=(bool)
    s3 = "abc";     // operator=(char[]) -> operator=(string&)
    s4 = v8;        // operator=(Value&)
    s5 = Value();   // operator=(Value&&)
    assert(s1.getType() == Type::kNumber && s1.parse() == "5.68");
    assert(s2.getType() == Type::kBoolean && s2.parse() == "false");
    assert(s3.getType() == Type::kString && s3.parse() == "\"abc\"");
    assert(s4.getType() == Type::kBoolean && s4.parse() == "true");
    assert(s5.getType() == Type::kNull && s5.parse() == "null");
    assert(s6.getType() == Type::kNumber && s6.parse() == "0x5");

    // 功能测试
    static_cast<Number&>(v2.get()).set(6.15);
    static_cast<Boolean&>(v3.get()).set(false);
    static_cast<String&>(v4.get()).set("ABC");
    assert(v2.getType() == Type::kNumber && v2.parse() == "6.15");
    assert(v3.getType() == Type::kBoolean && v3.parse() == "false");
    assert(v4.getType() == Type::kString && v4.parse() == "\"ABC\"");
    v2.reset();
    assert(v2.getType() == Type::kNull && v2.parse() == "null");
    v1.set(true);
    v2.set("abc");      // set(char[]) -> set(string&)
    v3.set(2);
    v4.set(3.5);
    assert(v4.getType() == Type::kNumber && v4.parse() == "3.5");
    assert(v3.getType() == Type::kNumber && v3.parse() == "2");
    assert(v1.getType() == Type::kBoolean && v1.parse() == "true");
    assert(v2.getType() == Type::kString && v2.parse() == "\"abc\"");
    v7.set(Object());
    v8.set(Array({1, 2 ,3}));
    assert(v7.getType() == Type::kObject && v7.parse() == "{}");
    assert(v8.getType() == Type::kArray && v8.parse() == "[1, 2, 3]");

    // 错误测试
    // static_cast<Boolean&>(v2.get()).set(false);     // 类型错误，行为未定义，在使用之前请检查类型!
    Value e;
    e.initFromJSON("qwe");      // 无法转化为任何类型
    e.initFromJSON("");         // 无法转化为任何类型
}

// 正则表达式测试
void RegExTest(){
    using namespace reg_ex;
    // Number 对象
    // 实数正则式
    assert(regex_match("1", kPatternNumber));
    assert(regex_match("1.1", kPatternNumber));
    assert(regex_match(".1", kPatternNumber));
    assert(regex_match("0.1", kPatternNumber));
    assert(regex_match("1.1e2", kPatternNumber));
    assert(regex_match("1.1e+2", kPatternNumber));
    assert(regex_match("1.1e-2", kPatternNumber));
    assert(regex_match("+1", kPatternNumber));
    assert(regex_match("+1.1", kPatternNumber));
    assert(regex_match("+.1", kPatternNumber));
    assert(regex_match("+0.1", kPatternNumber));
    assert(regex_match("+1.1e2", kPatternNumber));
    assert(regex_match("+1.1e+2", kPatternNumber));
    assert(regex_match("+1.1e-2", kPatternNumber));
    assert(regex_match("-1", kPatternNumber));
    assert(regex_match("-1.1", kPatternNumber));
    assert(regex_match("-.1", kPatternNumber));
    assert(regex_match("-0.1", kPatternNumber));
    assert(regex_match("-1.1e2", kPatternNumber));
    assert(regex_match("-1.1e+2", kPatternNumber));
    assert(regex_match("-1.1e-2", kPatternNumber));
    // 十六进制正则式
    assert(regex_match("0xabcdef123", kPatternHex));
    assert(regex_match("0Xabcdef123", kPatternHex));
    assert(regex_match("0xABCDEF123", kPatternHex));
    assert(!regex_match("123456789", kPatternHex));
    assert(regex_match("abcdef123", kPatternHex));
    assert(regex_match("ABCDEF123", kPatternHex));

    // String 对象 '...' 模式或 "..." 模式
    assert(regex_match("\"0xabcdef123\"", kPatternString));
    assert(regex_match("\"中文字符test\n\t\\\"", kPatternString));
    assert(regex_match("\"中文字符测试\"", kPatternString));
    assert(regex_match("\"This is a test \t !!!\"", kPatternString));
    assert(regex_match("\"\u235afasd\"", kPatternString));
    assert(regex_match("\'0xabcdef123\'", kPatternString));
    assert(regex_match("\'中文字符test\n\t\\\'", kPatternString));
    assert(regex_match("\'中文字符测试\'", kPatternString));
    assert(regex_match("\'This is a test \t !!!\'", kPatternString));
    assert(regex_match("\'\u235aunicode\'", kPatternString));

    // Boolean 对象
    assert(regex_match("true", kPatternBool));
    assert(regex_match("false", kPatternBool));
    assert(!regex_match("True", kPatternBool));
    assert(!regex_match("False", kPatternBool));

    // Null 对象
    assert(regex_match("null", kPatternNull));
    assert(!regex_match("Null", kPatternNull));

    // Array 对象 [...] 模式
    assert(regex_match("[1,2,4521,\"Test\nTest\",]", kPatternArray));

    // Object 对象 ..:.. 模式
    assert(regex_match("key: \"value\"", kPatternObj));
    assert(regex_match("\"key\": \'value\'", kPatternObj));
    assert(regex_match("key123_=+: \"value123_=\"", kPatternObj));
    assert(regex_match("\"key123_=+\": 1.234e-5", kPatternObj));
}

// Number 类测试，覆盖率 100%
void NumberTest(){
    // 五种初始化
    double d = 3.14;
    double i = 3;
    Number n1;                      // 默认为十进制 0
    Number n2(d);                   // Number(double)
    Number n2_(i);                  // Number(double) 自动转整数
    Number n3(n2);                  // Number(Number&)
    Number n4, n5;
    Number n6(3);                   // Number(int)
    n4.initFromJSON("0xabc123");    // 字符串初始化
    n5.initFromJSON("abc123");      // 字符串初始化
    assert(n1.get() == 0 && n1.parse() == "0" && n1.getType() == NumberType::kDefault);
    assert(n2.get() == 3.14 && n2.parse() == "3.14" && n2.getType() == NumberType::kDefault);
    assert(n2_.get() == 3 && n2_.parse() == "3" && n2_.getType() == NumberType::kInteger);
    assert(n3.get() == 3.14 && n3.parse() == "3.14" && n3.getType() == NumberType::kDefault);
    assert(n4.get() == 11256099 && n4.parse() == "0xabc123" && n4.getType() == NumberType::kHex);
    assert(n5.get() == 11256099 && n5.parse() == "0xabc123" && n5.getType() == NumberType::kHex);
    assert(n6.get() == 3 && n6.parse() == "3" && n6.getType() == NumberType::kInteger);

    // 两种赋值
    Number n7, n8;
    n7 = 3.14;                      // operator=(double)
    n8 = n4;                        // operator=(Number&)
    assert(n7.get() == 3.14 && n7.parse() == "3.14" && n7.getType() == NumberType::kDefault);
    assert(n8.get() == 11256099 && n8.parse() == "0xabc123" && n8.getType() == NumberType::kHex);

    // 功能测试
    n1.set(3.14159245622134131);
    assert(n1.get() == 3.14159245622134131 && n1.parse() == "3.14159" && n1.getType() == NumberType::kDefault);
    n1.parseSetting(NumberType::kFloat, 18);
    assert(n1.get() == 3.14159245622134131 && n1.parse() == "3.14159245622134131" && n1.getType() == NumberType::kFloat);
    n2.set(1234567890);
    assert(n2.get() == 1234567890 && n2.parse() == "1234567890" && n2.getType() == NumberType::kInteger);
    n4.parseSetting(NumberType::kInteger);
    assert(n4.get() == 11256099 && n4.parse() == "11256099" && n4.getType() == NumberType::kInteger);
    n4.reset();
    assert(n4.get() == 0 && n4.parse() == "0" && n4.getType() == NumberType::kDefault);
    n1.set(5);      // kFloat -> kInteger
    n2.set(5);      // kDefault -> kInteger
    n5.set(3.14);   // kHex -> kDefault
    n4.set(3.14);   // kInteger -> kDefault
    assert(n1.get() == 5 && n1.parse() == "5" && n1.getType() == NumberType::kInteger);
    assert(n2.get() == 5 && n1.parse() == "5" && n2.getType() == NumberType::kInteger);
    assert(n5.get() == 3.14 && n5.parse() == "3.14" && n5.getType() == NumberType::kDefault);
    assert(n4.get() == 3.14 && n4.parse() == "3.14" && n4.getType() == NumberType::kDefault);

    // 数字格式
    {
        Number s1, s2, s3, s4, s5, s6;
        s1.initFromJSON("1");
        s2.initFromJSON("1.1");
        s3.initFromJSON(".1");
        s4.initFromJSON("1.");
        s5.initFromJSON("0xa");
        s6.initFromJSON("1.1e2");
        assert(s1.get() == 1 && s1.parse() == "1" && s1.getType() == NumberType::kInteger);
        assert(s2.get() == 1.1 && s2.parse() == "1.1" && s2.getType() == NumberType::kDefault);
        assert(s3.get() == 0.1 && s3.parse() == "0.1" && s3.getType() == NumberType::kDefault);
        assert(s4.get() == 1 && s4.parse() == "1" && s4.getType() == NumberType::kInteger);
        assert(s5.get() == 10 && s5.parse() == "0xa" && s5.getType() == NumberType::kHex);
        assert(s6.get() == 110 && s6.parse() == "110" && s6.getType() == NumberType::kInteger);
    }
    {
        Number s1, s2, s3, s4, s5, s6;
        s1.initFromJSON("-1");
        s2.initFromJSON("-1.1");
        s3.initFromJSON("-.1");
        s4.initFromJSON("-1.");
        s5.initFromJSON("-0xa");
        s6.initFromJSON("-1.1e2");
        assert(s1.get() == -1 && s1.parse() == "-1" && s1.getType() == NumberType::kInteger);
        assert(s2.get() == -1.1 && s2.parse() == "-1.1" && s2.getType() == NumberType::kDefault);
        assert(s3.get() == -0.1 && s3.parse() == "-0.1" && s3.getType() == NumberType::kDefault);
        assert(s4.get() == -1 && s4.parse() == "-1" && s4.getType() == NumberType::kInteger);
        assert(s5.get() == -10 && s5.parse() == "-10" && s5.getType() == NumberType::kInteger);
        assert(s6.get() == -110 && s6.parse() == "-110" && s6.getType() == NumberType::kInteger);
    }

    // 错误测试
    Number n9;
    n9.initFromJSON("a.56");                    // 十六进制不支持小数，丢失精度
    assert(n9.get() == 10 && n9.parse() == "0xa" && n9.getType() == NumberType::kHex);
    Number n10;
    n10.initFromJSON("qabc5q.56");               // 非法的字符串
    n3.parseSetting(NumberType::kInteger);      // 浮点数以整数输出会丢失精度
    assert(n3.get() == 3.14 && n3.parse() == "3");
    n3.parseSetting(NumberType::kHex);          // 浮点数以十六进制输出会丢失精度
    assert(n3.get() == 3.14 && n3.parse() == "0x3");
    n3.initFromJSON("");
}

// Null 类测试，覆盖率 100%
void NullTest(){
    // 两种初始化
    Null n1, n2;
    n2.initFromJSON("null");
    assert(n1.parse() == "null");
    assert(n2.parse() == "null");

    // 错误测试
    Null n3;
    n3.initFromJSON("abc");     // 无法转成 Null 对象
    n3.initFromJSON("");        // 无法转成 Null 对象
}

// String 类测试，覆盖率 100%
void StringTest(){
    // 七种初始化
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
    s7.initFromJSON("\"initFromJSON\"");
    assert(s1.get() == "test\nnext\tline\n");
    assert(s1.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s1.parse() == "\"test\\nnext\\tline\\n\"");
    assert(s1.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s2.get() == "test\nnext\tline\n");
    assert(s2.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s2.parse() == "\"test\\nnext\\tline\\n\"");
    assert(s2.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s3.get() == "a String");
    assert(s3.getJSON() == "\"a String\"");
    assert(s3.parse() == "\"a String\"");
    assert(s3.getJSON() == "\"a String\"");
    assert(s4.get() == "a string");
    assert(s4.getJSON() == "\"a string\"");
    assert(s4.parse() == "\"a string\"");
    assert(s4.get() == "a string");
    assert(s5.get() == "");
    assert(s5.getJSON() == "\"\"");
    assert(s5.parse() == "\"\"");
    assert(s5.getJSON() == "\"\"");
    assert(s6.get() == "move String");
    assert(s6.getJSON() == "\"move String\"");
    assert(s6.parse() == "\"move String\"");
    assert(s6.getJSON() == "\"move String\"");
    assert(s7.get() == "initFromJSON");
    assert(s7.getJSON() == "\"initFromJSON\"");
    assert(s7.parse() == "\"initFromJSON\"");
    assert(s7.getJSON() == "\"initFromJSON\"");

    // 五种赋值
    String s8, s9, s10, s11;
    s7 = s1;                                      // operator=(String&)
    s8 = str;                                     // operator=(string&)
    s9 = std::move(String("test\nnext\tline\n")); // operator=(String&&)
    s10 = string("test\nnext\tline\n");           // operator=(string&&)
    s11 = "test\nnext\tline\n";                   // operator=(char[])
    assert(s7.get() == "test\nnext\tline\n");
    assert(s7.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s7.parse() == "\"test\\nnext\\tline\\n\"");
    assert(s7.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s8.get() == "test\nnext\tline\n");
    assert(s8.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s8.parse() == "\"test\\nnext\\tline\\n\"");
    assert(s8.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s9.get() == "test\nnext\tline\n");
    assert(s9.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s9.parse() == "\"test\\nnext\\tline\\n\"");
    assert(s9.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s10.get() == "test\nnext\tline\n");
    assert(s10.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s10.parse() == "\"test\\nnext\\tline\\n\"");
    assert(s10.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s11.get() == "test\nnext\tline\n");
    assert(s11.getJSON() == "\"test\\nnext\\tline\\n\"");
    assert(s11.parse() == "\"test\\nnext\\tline\\n\"");
    assert(s11.getJSON() == "\"test\\nnext\\tline\\n\"");

    // 功能测试
    s1.set("this is a message");
    assert(s1.get() == "this is a message");
    assert(s1.getJSON() == "\"this is a message\"");
    assert(s1.parse() == "\"this is a message\"");
    assert(s1.getJSON() == "\"this is a message\"");
    s1.set(string("abc"));
    assert(s1.get() == "abc");
    assert(s1.getJSON() == "\"abc\"");
    assert(s1.parse() == "\"abc\"");
    assert(s1.getJSON() == "\"abc\"");
    s1.reset();
    assert(s1.get() == "");
    assert(s1.getJSON() == "\"\"");
    assert(s1.parse() == "\"\"");
    assert(s1.getJSON() == "\"\"");

    // 转义字符测试
    String c;
    c.initFromJSON("\"\\\"\\b\\f\\t\\n\\r\\\\\\u002E\"");
    assert(c.get() == "\"\b\f\t\n\r\\0");
    assert(c.getJSON() == "\"\\\"\\b\\f\\t\\n\\r\\\\0\"");
    assert(c.parse() == "\"\\\"\\b\\f\\t\\n\\r\\\\0\"");
    assert(c.getJSON() == "\"\\\"\\b\\f\\t\\n\\r\\\\0\"");

    // 错误测试
    String e;
    e.initFromJSON("\"\"\"");     // '"' 未转义
    e.initFromJSON("\"");         // 长度非法，至少需要两个引号
    e.initFromJSON("\"\\p\"");    // \p 非法转义字符
    e.initFromJSON("\"\\u1\"");   // \u1 Unicode 非法
    e.initFromJSON("");           // 长度非法，至少需要两个引号
    assert(e.get() == "");
    assert(e.getJSON() == "\"\"");
    assert(e.parse() == "\"\"");
    assert(e.getJSON() == "\"\"");
}

// Boolean 类测试，覆盖率 100%
void BooleanTest(){
    // 五种初始化
    Boolean b1;                     // 默认为 false
    Boolean b2 = true;              // Boolean(bool)
    Boolean b3;
    Boolean b4(b2);                 // Boolean(Boolean&)
    b3.initFromJSON("false");       // 字符串初始化
    assert(!b1.get() && b1.parse() == "false");
    assert(b2.get() && b2.parse() == "true");
    assert(!b3.get() && b3.parse() == "false");
    assert(b4.get() && b4.parse() == "true");

    // 两种赋值
    Boolean b5, b6;
    b5 = b1;
    b6 = true;
    assert(!b5.get() && b5.parse() == "false");
    assert(b6.get() && b6.parse() == "true");

    // 功能测试
    b1.set(true);
    b2.reset();
    assert(b1.get() && b1.parse() == "true");
    assert(!b2.get() && b2.parse() == "false");

    // 错误测试
    Boolean e;
    e.initFromJSON("abc");      // "abc" 无法转化为 Boolean 对象
    e.initFromJSON("");         // "" 无法转化为 Boolean 对象
    assert(!e.get() && e.parse() == "false");
}

int main(){
    // Windows cmd 中文编码改为 UTF-8
    // system("chcp 65001");

    // ===================================================
    // 出现 [tiny_json_xxx] 的错误或警告请无视，只是测试流程
    // ===================================================

    NumberTest();
    BooleanTest();
    StringTest();
    NullTest();
    ValueTest();
    ArrayTest();
    ObjectTest();
    RegExTest();
    FuncTest();

    return 0;
}
