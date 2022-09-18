#include "tiny_json_frame.h"
#include <string>

/**************************
* @author   Yuan.
* @date     2022/9/18
* @brief    Log 类实现
***************************/

HANDLE tiny_json_log::Log::outHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void tiny_json_log::Log::error(std::string type, std::string info){
    setColor(Color::kRed);
    std::cout << "[tiny_json_Error_" + type + "]: " + info << std::endl;
}
void tiny_json_log::Log::warning(std::string type, std::string info){
    setColor(Color::kYellow);
    std::cout << "[tiny_json_Warning_" + type + "]: " + info << std::endl;
}
void tiny_json_log::Log::info(std::string type, std::string info){
    setColor(Color::kDefault);
    std::cout << "[tiny_json_Info_" + type + "]: " + info << std::endl;
}
void tiny_json_log::Log::check(std::string type, std::string info){
    setColor(Color::kGreen);
    std::cout << "[tiny_json_Info_" + type + "]: " + info << std::endl;
}
void tiny_json_log::Log::print(std::string str, Color color){
    setColor(color);
    std::cout << str << std::endl;
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

/**************************
* @author   Yuan.
* @date     2022/9/18
* @brief    Test 类实现
***************************/

bool tiny_json_test::Test::show_details_ = false;

tiny_json_test::Test::Test(std::string name) : name_(name) {
    Log::print("============================================");
    Log::print("Run Test: " + name_, Color::kYellow);
}
void tiny_json_test::Test::ExpectTrue(bool condition, std::string info){
    if(condition){
        if(show_details_)
            Log::print("ExpectTrue Check!", Color::kGreen);
        passed_count_++;
    }else{
        if(info.size() == 0){
            Log::print("Expected true but get false in case "
            + std::to_string(failed_count_ + passed_count_), Color::kRed);
        }else{
            Log::print("Expected true but get false in " + info, Color::kRed);
        }
        failed_count_++;
    }
}
void tiny_json_test::Test::ExpectFalse(bool condition, std::string info){
    if(!condition){
        if(show_details_)
            Log::print("ExpectFalse Check!", Color::kGreen);
        passed_count_++;
    }else{
        if(info.size() == 0){
            Log::print("Expected false but get true in case "
            + std::to_string(failed_count_ + passed_count_), Color::kRed);
        }else{
            Log::print("Expected false but get true in " + info, Color::kRed);
        }
        failed_count_++;
    }
}
void tiny_json_test::Test::ExpectInt(int expect, int number, std::string info){
    if(expect == number){
        if(show_details_)
            Log::print("ExpectInt Check!", Color::kGreen);
        passed_count_++;
    }else{
        if(info.size() == 0){
            Log::print("Expected " + std::to_string(expect) + " but get "
            + std::to_string(number) + " in case "
            + std::to_string(failed_count_ + passed_count_), Color::kRed);
        }else{
            Log::print("Expected " + std::to_string(expect) + " but get "
            + std::to_string(number) + " in " + info, Color::kRed);
        }
        failed_count_++;
    }
}
void tiny_json_test::Test::ExpectDouble(double expect, double number, std::string info){
    if(expect == number){
        if(show_details_)
            Log::print("ExpectDouble Check!", Color::kGreen);
        passed_count_++;
    }else{
        if(info.size() == 0){
            Log::print("Expected " + std::to_string(expect) + " but get "
            + std::to_string(number) + " in case "
            + std::to_string(failed_count_ + passed_count_), Color::kRed);
        }else{
            Log::print("Expected " + std::to_string(expect) + " but get "
            + std::to_string(number) + " in " + info, Color::kRed);
        }
        failed_count_++;
    }
}
void tiny_json_test::Test::ExpectString(std::string expect, std::string str, std::string info){
    if(expect == str){
        if(show_details_)
            Log::print("ExpectString Check!", Color::kGreen);
        passed_count_++;
    }else{
        if(info.size() == 0){
            Log::print("Expected " + expect + " but get "
            + str + " in case " + std::to_string(failed_count_ + passed_count_), Color::kRed);
        }else{
            Log::print("Expected " + expect + " but get " + str + " in " + info, Color::kRed);
        }
        failed_count_++;
    }
}
void tiny_json_test::Test::printResult(){
    int factor = (passed_count_ + failed_count_) == 0 ? 0 : passed_count_ * 100 / (passed_count_ + failed_count_);
    Log::print(std::to_string(passed_count_) + "/" + std::to_string(failed_count_ + passed_count_)
            + " Cases passed. (" + std::to_string(factor) + "%)");
}
tiny_json_test::Test::~Test(){
    printResult();
    if(failed_count_ == 0){
        Log::print("Test passed!", Color::kGreen);
    }else{
        Log::print("Test failed!", Color::kRed);
    }
    Log::print("==================Test End==================");
}

