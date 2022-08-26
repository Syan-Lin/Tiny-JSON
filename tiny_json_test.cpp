#include <iostream>
#include <string>
#include <assert.h>
#include "tiny_json.h"

using namespace std;
using namespace tiny_json;

// Number 类测试
void NumberTest(){
    // 四种初始化
    Number n1;                      // 默认为十进制 0
    Number n2(3.14);                // Number(double) 等价于 Number n2 = 3.14
    Number n3("0Xabc123");          // Number(char[]) 等价于 Number n3 = "abc123"
    Number n4(n2);                  // Number(Number&) 等价于 Number n4 = n2
    Number n5(string("abc123"));    // Number(string&) 等价于 Number n5 = string("abc123"
    assert(n1.get() == 0 && n1.parse() == "0");
    assert(n2.get() == 3.14 && n2.parse() == "3.14");
    assert(n3.get() == 11256099 && n3.parse() == "0xabc123");
    assert(n4.get() == 3.14 && n4.parse() == "3.14");
    assert(n5.get() == 11256099 && n5.parse() == "0xabc123");

    // 两种赋值
    Number n6, n7;
    n6 = 3.14;                      // operator=(double)
    n7 = n2;                        // operator=(Number&)
    assert(n6.get() == 3.14 && n6.parse() == "3.14");
    assert(n7.get() == 3.14 && n7.parse() == "3.14");

    // 功能测试
    n1.set(3.14159245622134131);
    assert(n1.get() == 3.14159245622134131 && n1.parse() == "3.14159");
    n1.parseSetting(NumberType::kFloat, 18);
    assert(n1.get() == 3.14159245622134131 && n1.parse() == "3.14159245622134131");
    n2.set(1234567890);
    assert(n2.get() == 1234567890 && n2.parse() == "1234567890");
    n3.parseSetting(NumberType::kInteger);
    assert(n3.get() == 11256099 && n3.parse() == "11256099");
    n3.reset();
    assert(n3.get() == 0 && n3.parse() == "0");

    // 数字格式
    {
        Number s1("1");
        Number s2("1.1");
        Number s3(".1");
        Number s4("1.");
        Number s5("0xa");
        Number s6("1.1e2");
        assert(s1.get() == 1 && s1.parse() == "1");
        assert(s2.get() == 1.1 && s2.parse() == "1.1");
        assert(s3.get() == 0.1 && s3.parse() == "0.1");
        assert(s4.get() == 1 && s4.parse() == "1");
        assert(s5.get() == 10 && s5.parse() == "0xa");
        assert(s6.get() == 110 && s6.parse() == "110");
    }
    {
        Number s1("-1");
        Number s2("-1.1");
        Number s3("-.1");
        Number s4("-1.");
        Number s5("-0xa");
        Number s6("-1.1e2");
        assert(s1.get() == -1 && s1.parse() == "-1");
        assert(s2.get() == -1.1 && s2.parse() == "-1.1");
        assert(s3.get() == -0.1 && s3.parse() == "-0.1");
        assert(s4.get() == -1 && s4.parse() == "-1");
        assert(s5.get() == -10 && s5.parse() == "-10");
        assert(s6.get() == -110 && s6.parse() == "-110");
    }

    // 错误测试
    Number n8("a.56");                          // 十六进制不支持小数，丢失精度
    assert(n8.get() == 10 && n8.parse() == "0xa");
    Number n9("qabc5q.56");                     // 非法的字符串
    n4.parseSetting(NumberType::kInteger);      // 浮点数以整数输出会丢失精度
    assert(n4.get() == 3.14 && n4.parse() == "3");
    n4.parseSetting(NumberType::kHex);          // 浮点数以十六进制输出会丢失精度
    assert(n4.get() == 3.14 && n4.parse() == "0x3");
}

// Null 类测试
void NullTest(){
    Null n1("null");
    Null n2;
    assert(n1.parse() == "null");
    assert(n2.parse() == "null");

    // 功能测试
    assert(n1.parseable("null"));
    assert(n1.parseable());

    // 错误测试
    Null n3("abc");
}

// String 类测试
void StringTest(){
    // 六种初始化
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
    assert(s1.get() == "test\nnext\tline\n");
    assert(s1.getJSON() == "test\\nnext\\tline\\n");
    assert(s1.parse() == "test\\nnext\\tline\\n");
    assert(s1.getJSON() == "test\\nnext\\tline\\n");
    assert(s2.get() == "test\nnext\tline\n");
    assert(s2.getJSON() == "test\\nnext\\tline\\n");
    assert(s2.parse() == "test\\nnext\\tline\\n");
    assert(s2.getJSON() == "test\\nnext\\tline\\n");
    assert(s3.get() == "a String");
    assert(s3.getJSON() == "a String");
    assert(s3.parse() == "a String");
    assert(s3.getJSON() == "a String");
    assert(s4.get() == "a string");
    assert(s4.getJSON() == "a string");
    assert(s4.parse() == "a string");
    assert(s4.get() == "a string");
    assert(s5.get() == "");
    assert(s5.getJSON() == "");
    assert(s5.parse() == "");
    assert(s5.getJSON() == "");
    assert(s6.get() == "move String");
    assert(s6.getJSON() == "move String");
    assert(s6.parse() == "move String");
    assert(s6.getJSON() == "move String");

    // 五种赋值
    String s7, s8, s9, s10, s11;
    s7 = s1;                                      // operator=(String&)
    s8 = str;                                     // operator=(string&)
    s9 = std::move(String("test\nnext\tline\n")); // operator=(String&&)
    s10 = string("test\nnext\tline\n");           // operator=(string&&)
    s11 = "test\nnext\tline\n";                   // operator=(char[])
    assert(s7.get() == "test\nnext\tline\n");
    assert(s7.getJSON() == "test\\nnext\\tline\\n");
    assert(s7.parse() == "test\\nnext\\tline\\n");
    assert(s7.getJSON() == "test\\nnext\\tline\\n");
    assert(s8.get() == "test\nnext\tline\n");
    assert(s8.getJSON() == "test\\nnext\\tline\\n");
    assert(s8.parse() == "test\\nnext\\tline\\n");
    assert(s8.getJSON() == "test\\nnext\\tline\\n");
    assert(s9.get() == "test\nnext\tline\n");
    assert(s9.getJSON() == "test\\nnext\\tline\\n");
    assert(s9.parse() == "test\\nnext\\tline\\n");
    assert(s9.getJSON() == "test\\nnext\\tline\\n");
    assert(s10.get() == "test\nnext\tline\n");
    assert(s10.getJSON() == "test\\nnext\\tline\\n");
    assert(s10.parse() == "test\\nnext\\tline\\n");
    assert(s10.getJSON() == "test\\nnext\\tline\\n");
    assert(s11.get() == "test\nnext\tline\n");
    assert(s11.getJSON() == "test\\nnext\\tline\\n");
    assert(s11.parse() == "test\\nnext\\tline\\n");
    assert(s11.getJSON() == "test\\nnext\\tline\\n");

    // 功能测试
    s1.set("this is a message");
    assert(s1.get() == "this is a message");
    assert(s1.getJSON() == "this is a message");
    assert(s1.parse() == "this is a message");
    assert(s1.getJSON() == "this is a message");
    s1.reset();
    assert(s1.get() == "");
    assert(s1.getJSON() == "");
    assert(s1.parse() == "");
    assert(s1.getJSON() == "");
    assert(s1.parseable("right"));
    assert(s1.parseable());
}

// Boolean 类测试
void BooleanTest(){
    // 五种初始化
    Boolean b1;                     // 默认为 false
    Boolean b2 = true;              // Boolean(bool) 等价于 Boolean b2(true)
    Boolean b3(string("false"));    // Boolean(string&) 等价于 Boolean b3 = string("false")
    Boolean b4(b2);                 // Boolean(Boolean&) 等价于 Boolean b4 = b2
    Boolean b5("true");             // Boolean(char[]) 等价于 Boolean b5 = "true"
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
    NumberTest();
    // BooleanTest();
    // StringTest();
    // NullTest();

    return 0;
}
