#pragma once

#include "../tiny_json.h"
#include "Windows.h"
#include <array>

namespace tiny_json_performance{

using namespace tiny_json;

class Performance{
public:
    void setScale(int, Type);
    void setScale(int);
    void run();
    void runLoop(int);
    DWORD getTime();
private:
    void printLine(std::string);
    void print(std::array<int, 6>);
    DWORD time_start_ = 0, time_end_ = 0;
    int array_num_ = 0;
    int string_num_ = 0;
    int bool_num_ = 0;
    int number_num_ = 0;
    int object_num_ = 0;
    int null_num_ = 0;
};

}