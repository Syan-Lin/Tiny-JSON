#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <regex>
#include "Test-Frame/tiny_json_frame.h"

namespace tiny_json{

using namespace tiny_json_log;

// 输出是否使用 JSON5 标准
// 注意事项：JSON5 下字符串请一律使用单引号包裹，且字符串中单引号需要转义
extern bool JSON5;
// 检查 JSON 格式错误，关闭会有更好的性能表现，开启则会自动执行格式检查
extern bool FORMAT_CHECK;

// JSON 数据类型
class Value;
class Object;
class Array;
class Number;
class String;
typedef decltype(nullptr) Null;

// JSON 数据类型枚举
enum class Type{
    kNumber,
    kBoolean,
    kString,
    kNull,
    kObject,
    kArray,
    kValue
};

enum class NumberType{
    kInteger,   // 整数类型
    kFloat,     // 浮点数类型，可指定位数
    kDefault,   // 默认类型，六位浮点数
    kHex        // 十六进制整数类型
};

namespace reg_ex{
    const std::regex kPatternNumber("[+-]?((\\d+|\\d*\\.\\d+)|(\\d+\\.\\d+)e[+-]?\\d+)");
    const std::regex kPatternHex("(0[xX]([a-fA-F\\d]*))|([a-fA-F]+[a-fA-F\\d]*)");
    const std::regex kPatternString("(\"(.|\\s)*\")|(\'(.|\\s)*\')");
    const std::regex kPatternBool("(true)|(false)");
    const std::regex kPatternNull("(null)");
    const std::regex kPatternArray("\\[(.|\\s)*\\]");
    const std::regex kPatternObj("(\"(.|\\s)+\":(.|\\s)+)|((.|\\s)+:(.|\\s)+)");
}

// 功能接口
class Parseable{
public:
    // 将对象输出为字符串
    virtual std::string parse() = 0;
    // 用字符串初始化对象
    virtual void initFromJSON(const std::string&) = 0;
};

// JSON 键值对中的值：
// 可以是 Number、Boolean、String、Null、Object、Array
class Value : public Parseable{
    typedef std::pair<std::string, Value> kv;
    typedef bool Boolean;
    typedef decltype(nullptr) Null;

    friend class Object;
    friend class Array;
public:
    // 拷贝控制成员
    Value();
    Value(Null);
    Value(const Object&);
    Value(Object&&) noexcept;
    Value(const Array&);
    Value(Array&&) noexcept;
    Value(std::initializer_list<kv>);    // Object 初始化
    Value(const double);                 // Number 初始化
    Value(const int);                    // Number 初始化
    Value(const bool);                   // Boolean 初始化
    Value(const std::string&);           // String 初始化
    Value(const char[]);                 // String 初始化
    Value(const Value&);                 // 拷贝构造
    Value(Value&&) noexcept;             // 移动构造
    Value& operator=(const Value&);      // 拷贝赋值
    Value& operator=(Value&&) noexcept;  // 移动赋值
    Value& operator=(const double);
    Value& operator=(const int);
    Value& operator=(const bool);
    Value& operator=(Null);
    Value& operator=(const std::string&);
    Value& operator=(const char[]);
    Value& operator[](const std::string&);
    Value& operator[](const size_t);
    ~Value();

    // 获取值类型
    Type getType() const;
    // 获取值
    template<typename T> T& get();
    // 重置值为 Null
    void reset();
    // 设置数字输出格式，当类型为 kInteger 或 kHex 时，第二个参数没有作用
    void parseSetting(NumberType, size_t = 6);

    // 将值输出为字符串
    std::string parse() override;
    // 用字符串初始化对象
    void initFromJSON(const std::string&) override;

private:
    explicit Value(std::string&&);
    Type type_;
    union{
        Null null_val_;
        Boolean bool_val_;
        Number* num_val_;
        String* str_val_;
        Array* arr_val_;
        Object* obj_val_;
    };
};

// JSON 的键值对集合
class Object : public Parseable{
    typedef std::string Key;
    typedef std::unordered_map<std::string, Value> kvMap;
    typedef std::pair<std::string, Value> kv;
public:
    // 拷贝控制成员
    Object() = default;
    Object(std::initializer_list<kv>);
    Object(const Object&);                // 拷贝构造
    Object(Object&&) noexcept;            // 移动构造
    Object& operator=(const Object&);     // 拷贝赋值
    Object& operator=(Object&&) noexcept; // 移动赋值
    // 若键值不存在，自动创建 null!
    Value& operator[](const Key&);
    ~Object() = default;

    // 添加键值对，如果存在同名的键值对，则覆盖
    void add(const Key&, const Value&);
    // 删除键值对，如果不存在则无行为
    void remove(const Key&);
    // 根据值类型
    Type getType(const Key&);
    // 是否有该键值
    bool has(const Key&);
    // Map 大小
    size_t size();
    // 清空键值对集合
    void clear();
    // 设置哈希表槽数
    void rehash(size_t);

    // 将键值对集合输出为字符串
    std::string parse() override;
    // 用字符串初始化对象
    void initFromJSON(const std::string&) override;

private:
    kvMap kv_map_;
    // 生成键值对
    void initKV(const std::string&);
};

// 存储 Value 的数组
class Array : public Parseable{
    typedef std::vector<Value> Vector;
public:
    // 拷贝控制成员
    Array() = default;
    // Array(std::initializer_list<Value>); // 列表初始化
    Array(Vector&&);                        // 列表初始化
    Array(const Array&);                    // 拷贝构造
    Array(Array&&) noexcept;                // 移动构造
    Array& operator=(const Array&);         // 拷贝赋值
    Array& operator=(Array&&) noexcept;     // 移动赋值
    Value& operator[](const size_t);
    ~Array() = default;

    // 在尾部添加元素
    void append(const Value&);
    void append(Value&&);
    // 在指定位置之前添加元素
    void add(size_t, const Value&);
    // 删除指定位置的元素
    void del(const size_t);
    // 删除尾部元素
    void pop();
    // 获取数组大小
    size_t size() const;
    // 清空数组
    void clear();

    // 将数组输出为字符串
    std::string parse() override;
    // 用字符串初始化对象
    void initFromJSON(const std::string&) override;

private:
    Vector arr_;
    // 检查数组越界
    bool checkIndex(const size_t index);
    bool checkIndexAdd(const size_t index);
    // 获取元素
    Value& get(const size_t);
};

// 数字类型，整数或浮点数，支持十六进制（负十六进制数会自动转化为整数）
class Number : public Parseable{
public:
    // 拷贝控制成员
    Number();
    Number(const Number&);
    Number(Number&&) noexcept;
    Number(const double);
    Number(const int);
    Number& operator=(const double);
    Number& operator=(const int);
    ~Number() = default;

    // 获取数字
    double& getDouble();
    int& getInt();
    // 是否是十六进制
    bool isHex() const;
    // 返回 parse 类型
    NumberType getType() const;

    // 将数字输出为字符串
    std::string parse() override;
    // 设置数字输出格式，当类型为 kInteger 或 kHex 时，第二个参数没有作用
    void parseSetting(NumberType, size_t = 6);
    // 用字符串初始化对象
    void initFromJSON(const std::string&) override;

private:
    void set(const double);
    union{
        int num_int_;
        double num_double_;
    };
    NumberType type_ = NumberType::kDefault;
    size_t decimal_place_ = 6;
};

// 字符串类型
// 特殊字符需要转义: \\n、\\t、\\r、\\u、\\、\"
// 换行 TODO
class String : public Parseable{
public:
    // 拷贝控制成员
    String();
    String(const String&);
    String(String&&) noexcept;
    String(const std::string&);
    String(const char[]);
    String(std::string&&) noexcept;             // 移动构造
    String& operator=(const std::string&);      // 拷贝赋值
    String& operator=(const char[]);            // 拷贝赋值
    String& operator=(std::string&&) noexcept;  // 移动赋值
    ~String() = default;

    // 获取字符串，注意如果获得引用，修改字符串的值不可带有转义字符
    // 如果需要带有转义字符，请使用其他设置字符串的接口
    std::string& get();
    // 获取 JSON 字符串
    std::string getJSON();

    // 输出字符串
    std::string parse() override;
    // 用字符串初始化对象，该字符串需要两个引号包裹
    void initFromJSON(const std::string&) override;

private:
    std::string str_;
    std::string parsed_str_;
    bool is_parsed_ = false;
    void parseForPrint();
    void parseForJSON();
};

// 将对象转化为字符串，是否进行格式化，默认为是
extern std::string parse(Object&, bool = true);
// 将字符串转化为对象
extern Object parse(std::string&);
// 判断字符串能否转化为对象
extern bool parseable(const std::string&, Type type = Type::kObject);
// 读取文件，参数为文件路径
extern Object readFile(const std::string&);
// 写入 JSON 文件，参数为文件路径和 JSON 对象
extern void writeFile(const std::string&, const std::string&);
// 去除注释
extern void removeAnnotation(std::string&);

}
