#include <iostream>
#include <string>
#include <assert.h>
#include <regex>
#include <Windows.h>
#include "tiny_json.h"

using namespace std;
using namespace tiny_json;

// Array 类测试
// TODO Object 和 Array的测试
void ArrayTest(){
    // 四种初始化
    Array a1;
    Array a2({3.14, false, Value(), "abc"});                // Array(initializer_list)
    Array a3(a2);                                           // Array(Array&)
    Array a4(move(Array({1, 2, 3})));                       // Array(Array&&)
    Array a5;
    a5.initFromJSON("[3.14, false, null, \"qwe\"]");        // 字符串初始化
    assert(a1.size() == 0 && a1.parse() == "[]");
    assert(a2.size() == 4 && a2.parse() == "[3.14, false, null, \"abc\"]");
    assert(a3.size() == 4 && a3.parse() == "[3.14, false, null, \"abc\"]");
    assert(a4.size() == 3 && a4.parse() == "[1, 2, 3]");
    assert(a5.size() == 4 && a5.parse() == "[3.14, false, null, \"qwe\"]");

    // 两种赋值
    Array a6, a7;
    a6 = a2;                                          // operator=(Array&)
    a7 = Array({1, 2, 3});       // operator=(Array&&)
    assert(a6.size() == 4 && a6.parse() == "[3.14, false, null, \"abc\"]");
    assert(a7.size() == 3 && a7.parse() == "[1, 2, 3]");

    // 功能测试
    a7[0] = 6.28;           // 等价于 a7[0].set(6.28)
    a7[1].set(true);        // 等价于 a7[1] = true
    a7.append(Value(5));    // append(Value&);
    a7.append("hello");     // append(string&);
    a7.append(false);       // append(bool);
    a7.append(3.14);        // append(double);
    a7.append(3);           // append(int);
    a7.append();            // append();
    assert(a7.size() == 9 && a7.parse() == "[6.28, true, 3, 5, \"hello\", false, 3.14, 3, null]");
    a7.reset();
    assert(a7.size() == 0 && a7.parse() == "[]");
    a7.add(0);                  // add(size_t)
    a7.add(1, Value(false));    // add(size_t, Value&)
    a7.add(2, "abc");           // add(size_t, string&)
    a7.add(2, true);            // add(size_t, bool)
    a7.add(4, 4);               // add(size_t, int)
    a7.add(5, 6.3);             // add(size_t, double)
    assert(a7.size() == 6 && a7.parse() == "[null, false, true, \"abc\", 4, 6.3]");
    a7.set(1);                  // set(size_t)
    a7.set(0, 1);               // set(size_t, int)
    a7.set(2, 1.1);             // set(size_t, double)
    a7.set(3, true);            // set(size_t, bool)
    a7.set(4, "hello");         // set(size_t, string&)
    a7.set(5, Value());         // set(size_t, Value&)
    assert(a7.size() == 6 && a7.parse() == "[1, null, 1.1, true, \"hello\", null]");
    a7.del(0);
    a7.del(1);
    a7.pop();
    assert(a7.size() == 3 && a7.parse() == "[null, true, \"hello\"]");

    // 错误测试
    Array e;
    // 数组越界
    // e.pop();
    // e.del(1);
    // e.set(2, 4);
    // e[5] = 6;
    e.initFromJSON("错误字符串，无法初始化");
    e.initFromJSON("['asd\", 2, null]");
}

// Value 类测试
// TODO Object 和 Array的测试
// TODO 基础类型初始化
void ValueTest(){
    // 九种初始化
    Value v1;                       // 默认为 null
    Value v2(3.14);                 // Value(double) 等价于 Value v2 = 3.14
    Value v3(true);                 // Value(bool) 等价于 v3 = true
    Value v4("abc");                // Value(string&) 等价于 Value v5 = "abc"
    Value v5(Array({1, 2, 3, false, "123"}));
    // TODO
    // Value v6(Object());
    // Value v6_ = Object();
    Value v7(v2);                   // Value(Value&) 等价于 Value v8 = v2
    Value v8(move(Value(true)));    // Value(Value&&) 等价于 Value v7 = Value(true)
    assert(v1.getType() == Type::kNull && v1.parse() == "null");
    assert(v2.getType() == Type::kNumber && v2.parse() == "3.14");
    assert(v3.getType() == Type::kBoolean && v3.parse() == "true");
    assert(v4.getType() == Type::kString && v4.parse() == "\"abc\"");
    assert(v5.getType() == Type::kArray && v5.parse() == "[1, 2, 3, false, \"123\"]");
    assert(v7.getType() == Type::kNumber && v7.parse() == "3.14");
    assert(v8.getType() == Type::kBoolean && v8.parse() == "true");

    // 字符串初始化
    Value s1, s2, s3, s4, s5, s6;
    s1.initFromJSON("3.14");        // 初始化为 Number 类型
    s2.initFromJSON("true");        // 初始化为 Boolean 类型
    s3.initFromJSON("null");        // 初始化为 Null 类型
    s4.initFromJSON("\"abc\"");     // 初始化为 String 类型
    // s5.initFromJSON("[1, 2, 3]");          // 初始化为 Array 类型
    // Value s1("\"a\": 1");           // 初始化为 Object 类型
    assert(s1.getType() == Type::kNumber && s1.parse() == "3.14");
    assert(s2.getType() == Type::kBoolean && s2.parse() == "true");
    assert(s3.getType() == Type::kNull && s3.parse() == "null");
    assert(s4.getType() == Type::kString && s4.parse() == "\"abc\"");

    // 五种赋值
    s1 = 5.68;      // operator=(double)
    s2 = false;     // operator=(bool)
    s3 = "abc";     // operator=(string&)
    s4 = v8;
    s5 = Value();
    assert(s1.getType() == Type::kNumber && s1.parse() == "5.68");
    assert(s2.getType() == Type::kBoolean && s2.parse() == "false");
    assert(s3.getType() == Type::kString && s3.parse() == "\"abc\"");
    assert(s4.getType() == Type::kBoolean && s4.parse() == "true");
    assert(s5.getType() == Type::kNull && s5.parse() == "null");

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
    v2.set("abc");
    v3.set(2);
    v4.set(3.5);
    assert(v4.getType() == Type::kNumber && v4.parse() == "3.5");
    assert(v3.getType() == Type::kNumber && v3.parse() == "2");
    assert(v1.getType() == Type::kBoolean && v1.parse() == "true");
    assert(v2.getType() == Type::kString && v2.parse() == "\"abc\"");

    // 错误测试
    // static_cast<Boolean&>(v2.get()).set(false);     // 类型错误，行为未定义，在使用之前请检查类型!
    Value e;
    e.initFromJSON("qwe");                          // 无法转化为任何类型
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

// Number 类测试
void NumberTest(){
    // 五种初始化
    Number n1;                      // 默认为十进制 0
    Number n2(3.14);                // Number(double) 等价于 Number n2 = 3.14
    Number n3(n2);                  // Number(Number&) 等价于 Number n4 = n2
    Number n4, n5;
    n4.initFromJSON("0xabc123");    // 字符串初始化
    n5.initFromJSON("abc123");      // 字符串初始化
    assert(n1.get() == 0 && n1.parse() == "0");
    assert(n2.get() == 3.14 && n2.parse() == "3.14");
    assert(n3.get() == 3.14 && n3.parse() == "3.14");
    assert(n4.get() == 11256099 && n4.parse() == "0xabc123");
    assert(n5.get() == 11256099 && n5.parse() == "0xabc123");

    // 两种赋值
    Number n6, n7;
    n6 = 3.14;                      // operator=(double)
    n7 = n4;                        // operator=(Number&)
    assert(n6.get() == 3.14 && n6.parse() == "3.14");
    assert(n7.get() == 11256099 && n7.parse() == "0xabc123");

    // 功能测试
    n1.set(3.14159245622134131);
    assert(n1.get() == 3.14159245622134131 && n1.parse() == "3.14159");
    n1.parseSetting(NumberType::kFloat, 18);
    assert(n1.get() == 3.14159245622134131 && n1.parse() == "3.14159245622134131");
    n2.set(1234567890);
    assert(n2.get() == 1234567890 && n2.parse() == "1234567890");
    n4.parseSetting(NumberType::kInteger);
    assert(n4.get() == 11256099 && n4.parse() == "11256099");
    n4.reset();
    assert(n4.get() == 0 && n4.parse() == "0");

    // 数字格式
    {
        Number s1, s2, s3, s4, s5, s6;
        s1.initFromJSON("1");
        s2.initFromJSON("1.1");
        s3.initFromJSON(".1");
        s4.initFromJSON("1.");
        s5.initFromJSON("0xa");
        s6.initFromJSON("1.1e2");
        assert(s1.get() == 1 && s1.parse() == "1");
        assert(s2.get() == 1.1 && s2.parse() == "1.1");
        assert(s3.get() == 0.1 && s3.parse() == "0.1");
        assert(s4.get() == 1 && s4.parse() == "1");
        assert(s5.get() == 10 && s5.parse() == "0xa");
        assert(s6.get() == 110 && s6.parse() == "110");
    }
    {
        Number s1, s2, s3, s4, s5, s6;
        s1.initFromJSON("-1");
        s2.initFromJSON("-1.1");
        s3.initFromJSON("-.1");
        s4.initFromJSON("-1.");
        s5.initFromJSON("-0xa");
        s6.initFromJSON("-1.1e2");
        assert(s1.get() == -1 && s1.parse() == "-1");
        assert(s2.get() == -1.1 && s2.parse() == "-1.1");
        assert(s3.get() == -0.1 && s3.parse() == "-0.1");
        assert(s4.get() == -1 && s4.parse() == "-1");
        assert(s5.get() == -10 && s5.parse() == "-10");
        assert(s6.get() == -110 && s6.parse() == "-110");
    }

    // 错误测试
    Number n8;
    n8.initFromJSON("a.56");                    // 十六进制不支持小数，丢失精度
    assert(n8.get() == 10 && n8.parse() == "0xa");
    Number n9;
    n9.initFromJSON("qabc5q.56");               // 非法的字符串
    n3.parseSetting(NumberType::kInteger);      // 浮点数以整数输出会丢失精度
    assert(n3.get() == 3.14 && n3.parse() == "3");
    n3.parseSetting(NumberType::kHex);          // 浮点数以十六进制输出会丢失精度
    assert(n3.get() == 3.14 && n3.parse() == "0x3");
}

// Null 类测试
void NullTest(){
    // 两种初始化
    Null n1, n2;
    n2.initFromJSON("null");
    assert(n1.parse() == "null");
    assert(n2.parse() == "null");

    // 功能测试
    assert(n1.parseable("null"));
    assert(n1.parseable());

    // 错误测试
    Null n3;
    n3.initFromJSON("abc");     // 无法转成 Null 对象
}

// String 类测试
void StringTest(){
    // 七种初始化
    string str = "test\nnext\tline\n";
    String s1(str);                             // String(string&) 等价于 String s1 = str
    String s2(s1);                              // String(String&) 等价于 String s2 = s1
    String s3("a String");                      // String(char[]) -> String(string&&)
                                                // 等价于 String s3(String("a String"))
                                                // 等价于 String s3 = String("a String")
                                                // 等价于 String s3 = "a String"
    String s4(string("a string"));              // String(string&&) 等价于 String s4 = string("a string")
    String s5;                                  // String()
    String s6(move(String("move String")));     // String(String&&) 等价于 String s6 = move(String("move String"))
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
    s1.reset();
    assert(s1.get() == "");
    assert(s1.getJSON() == "\"\"");
    assert(s1.parse() == "\"\"");
    assert(s1.getJSON() == "\"\"");
    assert(s1.parseable("right"));
    assert(s1.parseable());
}

// Boolean 类测试
void BooleanTest(){
    // 五种初始化
    Boolean b1;                     // 默认为 false
    Boolean b2 = true;              // Boolean(bool) 等价于 Boolean b2(true)
    Boolean b3;    // Boolean(string&) 等价于 Boolean b3 = string("false")
    Boolean b4(b2);                 // Boolean(Boolean&) 等价于 Boolean b4 = b2
    Boolean b5("true");             // Boolean(char[]) 等价于 Boolean b5 = "true"
    b3.initFromJSON("false");       // 字符串初始化
    assert(!b1.get());
    assert(b2.get());
    assert(!b3.get());
    assert(b4.get());
    assert(b5.get());

    // 一种赋值
    Boolean b6;
    b6 = b1;
    assert(!b6.get());

    // 功能测试
    b1.set(true);
    b2.reset();
    assert(b1.get());
    assert(!b2.get());
    assert(b1.parseable());
    assert(!b1.parseable("can't parse"));
    assert(b1.parse() == "true");
    assert(b2.parse() == "false");
    // 错误测试
    Boolean b = "abc";
}

int main(){
    // cmd 中文编码改为 UTF-8
    // system("chcp 65001");

    NumberTest();
    BooleanTest();
    StringTest();
    NullTest();
    RegExTest();
    ValueTest();
    ArrayTest();

    return 0;
}
