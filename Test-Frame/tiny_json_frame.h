#pragma once

#include <iostream>
#include "Windows.h"
#include <array>

namespace tiny_json_log{

enum class Color{
    kDefault,
    kRed,
    kYellow,
    kGreen
};

class Log{
public:
    // 打印错误信息
    static void error(std::string, std::string);
    // 打印警告信息
    static void warning(std::string, std::string);
    // 打印日志信息
    static void info(std::string, std::string);
    // 打印确认信息
    static void check(std::string, std::string);
    // 打印信息
    static void print(std::string, Color = Color::kDefault);
    static void setColor(Color);
private:
    static HANDLE outHandle;
};

};

namespace tiny_json_test{

using namespace tiny_json_log;

class Test{
public:
    Test(std::string);
    ~Test();
    void printResult();
    // 断言
    void ExpectTrue(bool, std::string = "");
    void ExpectFalse(bool, std::string = "");
    void ExpectInt(int, int, std::string = "");
    void ExpectDouble(double, double, std::string = "");
    void ExpectString(std::string, std::string, std::string = "");

public:
    const std::string name_;
    int failed_count_ = 0;
    int passed_count_ = 0;
    static bool show_details_;
};

}
