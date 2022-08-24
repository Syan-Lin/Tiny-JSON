#include <iostream>
#include <string>
#include <assert.h>
#include "tiny_json.h"

using namespace std;
using namespace tiny_json;

// Number 类测试
void NumberTest(){
    // 六种初始化
    Number n1;
    Number n2(3.14);
    Number n3("abc123");    // 十六进制
    Number n4(n2);
    Number n5 = n3;
    Number n6 = 7.8;
    cout << n2.get() << " " << n2.parse() << endl;
    cout << n3.get() << " " << n3.parse() << endl;
    cout << n4.get() << " " << n4.parse() << endl;
    cout << n5.get() << " " << n5.parse() << endl;
    cout << n6.get() << " " << n6.parse() << endl;

    // 功能测试
    n1.set(3.141592655555555);
    cout << n1.get() << ' ' << n1.parse() << endl;
    n1.parseSetting(NumberType::kFloat, 18);
    cout << n1.get() << ' ' << n1.parse() << endl;
    n2.set(1234567890);
    cout << n2.get() << ' ' << n2.parse() << endl;
    n2.parseSetting(NumberType::kInteger);
    cout << n2.get() << ' ' << n2.parse() << endl;

    Number n7("-1.23457e+09");
    n7.parseSetting(NumberType::kInteger);
    cout << static_cast<int>(n7.get()) << ' ' << n7.parse() << endl;

    // 错误测试
    Number n8("a.56"); // 十六进制不支持小数，丢失精度
    cout << n8.get() << ' ' << n8.parse() << endl;
    Number n9("qabc5q.56");
}

// Null 类测试
void NullTest(){
    Null n1("null");
    Null n2;
    cout << n1.parse() << endl;
    cout << n2.parse() << endl;

    // 功能测试
    assert(n1.parseable("null"));
    assert(n1.parseable());

    // 错误测试
    Null n3("abc");
}

// String 类测试
void StringTest(){
    // 九种初始化
    string str = "test\nnext\tline\n";
    String s1(str);
    String s2(s1);
    String s3(String("a String"));
    String s4("a string");
    String s5 = s1;
    String s6 = String("a String");
    String s7 = str;
    String s8 = "a string";
    String s9;
    cout << "s1: " << s1.get() << " parse:" << s1.parse() << endl;
    cout << "s2: " << s2.get() << " parse:" << s2.parse() << endl;
    cout << "s3: " << s3.get() << " parse:" << s3.parse() << endl;
    cout << "s4: " << s4.get() << " parse:" << s4.parse() << endl;
    cout << "s5: " << s5.get() << " parse:" << s5.parse() << endl;
    cout << "s6: " << s6.get() << " parse:" << s6.parse() << endl;
    cout << "s7: " << s7.get() << " parse:" << s7.parse() << endl;
    cout << "s8: " << s8.get() << " parse:" << s8.parse() << endl;

    // 功能测试
    s1.set("this is a message");
    cout << "s1: " << s1.get() << " parse:" << s1.parse() << endl;
    s1.reset();
    cout << "s1: " << s1.get() << " parse:" << s1.parse() << endl;
    assert(s1.parseable("right"));
    assert(s1.parseable());
}

// Boolean 类测试
void BooleanTest(){
    // 五种初始化
    Boolean b1;
    Boolean b2 = true;
    Boolean b3(string("false"));
    Boolean b4(b2);
    Boolean b5 = b2;
    Boolean b6("true");
    assert(!b1.get());
    assert(b2.get());
    assert(!b3.get());
    assert(b4.get());
    assert(b5.get());
    assert(b6.get());
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
    BooleanTest();
    StringTest();
    NullTest();

    return 0;
}
