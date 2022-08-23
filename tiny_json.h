#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <vector>

namespace tiny_json{

// 是否输出调试信息
extern bool DEBUG;
// 是否使用 JSON5 标准
extern bool JSON5;

// JSON 数据类型
class Value;
class Object;
class Array;
class Number;
class Null;
class String;
class Boolean;

// JSON 数据类型枚举
enum class Type{
    kNumber,
    kBoolean,
    kString,
    kNull,
    kObject,
    kArray
};

enum class NumberType{
    kInteger,   // 整数类型
    kFloat,     // 浮点数类型，可指定位数
    kDefault    // 默认类型，六位浮点数
};

// 功能接口
class Parseable{
public:
    // 将对象输出为字符串
    virtual std::string parse() = 0;
    // JSON 文件是否能转化为对象
    virtual bool parseable(std::string) const = 0;
    // 对象是否能转化为 JSON 文件
    virtual bool parseable() const = 0;
};

// JSON 键值对中的值：
// 可以是 Number、Boolean、String、Null、Object、Array
class Value : public Parseable{
    template <typename T> using ptr = std::unique_ptr<T>;
public:
    // 拷贝控制成员
    Value();
    Value(const Number&);
    Value(const Boolean&);
    Value(const Null&);
    Value(const String&);
    Value(const Object&);
    Value(const Array&);
    Value(const Value&);                // 拷贝构造
    Value(Value&&) noexcept;            // 移动构造
    Value& operator=(Value&);           // 拷贝赋值
    Value& operator=(Value&&) noexcept; // 移动赋值
    ~Value();

    // 值设置
    void set(const Number&);
    void set(const Boolean&);
    void set(const Null&);
    void set(const String&);
    void set(const Object&);
    void set(const Array&);
    void set(const Value&);
    // 获取值类型
    Type getType() const;
    // 获取值
    template <typename T> T& get() const;
    // 重置值为 Null
    void reset();

    // 将值输出为字符串
    std::string parse() override;

private:
    Type type_;
    union{
        ptr<Number> number_val_;
        ptr<Boolean> boolean_val_;
        ptr<String> string_val_;
        ptr<Null> null_val_;
        ptr<Object> object_val_;
        ptr<Array> array_val_;
    };
};

// JSON 的键值对集合
class Object : public Parseable{
    typedef std::string Key;
    typedef std::map<std::string, std::unique_ptr<Value>> kvMap;
public:
    // 拷贝控制成员
    Object();
    Object(const Key&, const Value&);
    Object(const Object&);                // 拷贝构造
    Object(Object&&) noexcept;            // 移动构造
    Object& operator=(Object&);           // 拷贝赋值
    Object& operator=(Object&&) noexcept; // 移动赋值
    ~Object();

    // 添加键值对，如果存在同名的键值对，则覆盖
    void add(const Key&, const Value&);
    // 删除键值对，如果不存在则无行为
    void del(const Key&);
    // 设置键值对，如果不存在则创建一个
    void set(const Key&, const Value&);
    // 根据键获得值
    Value& get(const Key&) const;
    // 清空键值对集合
    void reset();

    // 将键值对集合输出为字符串
    std::string parse() override;

private:
    kvMap kv_map_;
};

// 存储 Value 的数组
class Array : public Parseable{
    typedef std::vector<std::unique_ptr<Value>> Vector;
public:
    // 拷贝控制成员
    Array();
    Array(std::initializer_list<Value>);    // 列表初始化
    Array(const Array&);                    // 拷贝构造
    Array(Array&&) noexcept;                // 移动构造
    Array& operator=(Array&);               // 拷贝赋值
    Array& operator=(Array&&) noexcept;     // 移动赋值
    ~Array();

    // 在尾部添加元素
    void append(const Value&);
    // 删除尾部元素
    void pop();
    // 在指定位置之前添加元素
    void add(size_t, const Value&);
    // 删除指定位置的元素
    void del(size_t, const Value&);
    // 设置指定位置的元素
    void set(size_t, const Value&);
    // 获取数组大小
    size_t size() const;
    // 获取元素
    Value& get(size_t) const;
    // 清空数组
    void reset();

    // 将数组输出为字符串
    std::string parse() override;

private:
    Vector arr_;
};

// 数字类型，整数或浮点数
class Number : public Parseable{
public:
    // 拷贝控制成员
    Number() = default;
    Number(const double);
    Number(const Number&);                    // 拷贝构造
    Number& operator=(const Number&);         // 拷贝赋值
    Number& operator=(const double);
    ~Number() = default;

    // 设置数字
    void set(const double);
    // 获取数字
    double get() const;
    // 重置数字
    void reset();

    // 将数字输出为字符串
    std::string parse() override;
    // JSON 对象能够转化为 Number 对象
    bool parseable(std::string) const override;
    // Number 对象能否转化为 JSON 对象
    bool parseable() const override;
    // 设置数字输出格式
    void parseSetting(NumberType, size_t = 6);

private:
    double num_;
    NumberType type_ = NumberType::kDefault;
    size_t decimal_place_ = 6;
};

// 空类型
class Null : public Parseable{
public:
    // 输出 Null 字符串
    std::string parse() override;
    // JSON 对象能够转化为 Null 对象
    bool parseable(std::string) const override;
    // Null 对象能否转化为 JSON 对象
    bool parseable() const override;
};

// 字符串类型
// 特殊字符需要转义: \\n、\\t、\\r、\\u、\\、\"
class String : public Parseable{
public:
    // 拷贝控制成员
    String() = default;
    String(const std::string&);
    String(const String&);                      // 拷贝构造
    String(String&&) noexcept;                  // 移动构造
    String(std::string&&) noexcept;             // 移动构造
    String& operator=(const String&);           // 拷贝赋值
    String& operator=(String&&) noexcept;       // 移动赋值
    String& operator=(const std::string&);      // 拷贝赋值
    String& operator=(std::string&&) noexcept;  // 移动赋值
    ~String() = default;

    // 设置字符串
    void set(const std::string&);
    // 获取字符串
    std::string get() const;
    // 重置字符串
    void reset();

    // 输出字符串
    std::string parse() override;
    // JSON 对象能够转化为 String 对象
    bool parseable(std::string) const override;
    // String 对象能否转化为 JSON 对象
    bool parseable() const override;

private:
    std::string str_;
    std::string parsed_str_;
    bool is_parsed_ = false;
    void parseForPrint();
    void parseForFile();
};

// 布尔类型
class Boolean : public Parseable{
public:
    // 拷贝控制成员
    Boolean() = default;
    Boolean(const bool);
    Boolean(const std::string&);
    Boolean(const Boolean&);                    // 拷贝构造
    Boolean& operator=(const Boolean&);         // 拷贝赋值
    ~Boolean() = default;

    // 设置布尔值
    void set(const bool);
    // 获取布尔值
    bool get() const;
    // 重置布尔值
    void reset();

    // 将布尔值输出为字符串
    std::string parse() override;
    // JSON 对象能够转化为 Boolean 对象
    bool parseable(std::string) const override;
    // Boolean 对象能否转化为 JSON 对象
    bool parseable() const override;

private:
    bool bool_;
};

// 将对象转化为字符串
extern std::string parse(const Object&);
// 将字符串转化为对象
extern Object& parse(const std::string);

}
