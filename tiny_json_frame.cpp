#include "tiny_json_frame.h"

/**************************
* @author   Yuan.
* @date     2022/9/18
* @brief    Log 类实现
***************************/

HANDLE tiny_json_log::Log::outHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void tiny_json_log::Log::error(std::string type, std::string info){
    setColor(Color::kRed);
    std::cout << "[tiny_json_Error_" + type + "]: " + info << std::endl;
    setColor(Color::kDefault);
}
void tiny_json_log::Log::warning(std::string type, std::string info){
    setColor(Color::kYellow);
    std::cout << "[tiny_json_Warning_" + type + "]: " + info << std::endl;
    setColor(Color::kDefault);
}
void tiny_json_log::Log::info(std::string type, std::string info){
    setColor(Color::kDefault);
    std::cout << "[tiny_json_Info_" + type + "]: " + info << std::endl;
}
void tiny_json_log::Log::check(std::string type, std::string info){
    setColor(Color::kGreen);
    std::cout << "[tiny_json_Info_" + type + "]: " + info << std::endl;
    setColor(Color::kDefault);
}
void tiny_json_log::Log::print(std::string str, Color color){
    setColor(color);
    std::cout << str << std::endl;
    setColor(Color::kDefault);
}
void tiny_json_log::Log::setColor(Color color){
    switch(color){
        case Color::kDefault:
            SetConsoleTextAttribute(outHandle,0x07);
            break;
        case Color::kRed:
            SetConsoleTextAttribute(outHandle,0x04);
            break;
        case Color::kYellow:
            SetConsoleTextAttribute(outHandle,0x06);
            break;
        case Color::kGreen:
            SetConsoleTextAttribute(outHandle,0x02);
            break;
    }
}