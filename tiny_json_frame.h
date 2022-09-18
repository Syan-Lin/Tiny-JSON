#pragma once

#include <iostream>
#include "Windows.h"

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
    static void print(std::string, Color);
private:
    static HANDLE outHandle;
    static void setColor(Color);
};

};

namespace tiny_json_test{

using namespace tiny_json_log;


}
