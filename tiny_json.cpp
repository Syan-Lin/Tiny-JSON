
/**************************
* @author   Yuan.
* @date     2022/8/24
* @brief    tiny_json 实现
* @version  v0.1 alpha
***************************/

#include "tiny_json.h"
#include <sstream>
#include <iomanip>
#include <iostream>

bool tiny_json::DEBUG = false;
bool tiny_json::JSON5 = true;

// 判断对象是否可转化为字符串（可能出现环导致死循环）
static bool parseable(const tiny_json::Object&){
    return true;
}
// 判断字符串是否可转化为对象
static bool parseable(std::string){
    return true;
}
std::string tiny_json::parse(const tiny_json::Object& obj){
    return "hhh";
}
// tiny_json::Object& tiny_json::parse(const std::string str){
//     return Object();
// }

/**************************
* @author   Yuan.
* @date     2022/8/24
* @brief    Number 类实现
***************************/

// 拷贝控制成员
tiny_json::Number::Number(const double val){ num_ = val; }
tiny_json::Number::Number(const Number& val){ num_ = val.get(); }
tiny_json::Number& tiny_json::Number::operator=(const Number& val){
    num_ = val.get();
    return *this;
}
tiny_json::Number& tiny_json::Number::operator=(const double val){
    num_ = val;
    return *this;
}

// 功能成员
void tiny_json::Number::set(const double val){ num_ = val; }
double tiny_json::Number::get() const { return num_; }
void tiny_json::Number::reset(){ num_ = 0; }

std::string tiny_json::Number::parse(){
    std::string str;
    std::stringstream ss;

    switch(type_){
        case NumberType::kDefault:{
            ss << num_;
            str = ss.str();
            break;
        }
        case NumberType::kInteger:{
            long long integer = static_cast<long long>(num_);
            ss << integer;
            str = ss.str();
            break;
        }
        case NumberType::kFloat:{
            ss << std::setprecision(decimal_place_) << num_;
            str = ss.str();
            break;
        }
    }
    return str;
}
bool tiny_json::Number::parseable(std::string val) const {
    return true;
}
bool tiny_json::Number::parseable() const { return true; }
void tiny_json::Number::parseSetting(NumberType type, size_t decimal_place){
    type_ = type;
    decimal_place_ = decimal_place;
}

/**************************
* @author   Yuan.
* @date     2022/8/23
* @brief    Null 类实现
***************************/

std::string tiny_json::Null::parse(){ return "null"; }
bool tiny_json::Null::parseable(std::string val) const { return val == "null" ? true : false; }
bool tiny_json::Null::parseable() const { return true; }

/**************************
* @author   Yuan.
* @date     2022/8/24
* @brief    String 类实现
***************************/

// 拷贝控制成员
tiny_json::String::String(const std::string& val){
    str_ = val;
    parse();
}
tiny_json::String::String(const String& val){ str_ = val.get(); }
tiny_json::String::String(String&& val) noexcept{ str_ = std::move(val.get()); }
tiny_json::String::String(std::string&& val) noexcept{
    str_ = std::move(val);
    parse();
}
tiny_json::String& tiny_json::String::operator=(const String& val){
    str_ = val.get();
    return *this;
}
tiny_json::String& tiny_json::String::operator=(String&& val) noexcept{
    str_ = std::move(val.get());
    return *this;
}
tiny_json::String& tiny_json::String::operator=(const std::string& val){
    str_ = val;
    parse();
    return *this;
}
tiny_json::String& tiny_json::String::operator=(std::string&& val) noexcept{
    str_ = std::move(val);
    parse();
    return *this;
}

// 功能成员
void tiny_json::String::set(const std::string& val){
    str_ = val;
    is_parsed_ = true;
}
std::string tiny_json::String::get() const { return str_; }
void tiny_json::String::reset(){ str_.clear(); }

std::string tiny_json::String::parse() {
    if(is_parsed_){
        return parsed_str_;
    }else if(!parseable()){
        return "";
    }
    is_parsed_ = true;
    parseForPrint();
    parseForFile();

    return parsed_str_;
}
void tiny_json::String::parseForPrint(){
    std::string str;
    for(int i = 0; i < str_.size(); i++){
        if(str_[i] == '\\'){
            i++;
            switch(str_[i]){
                case 'b':
                    str.push_back('\b');
                    break;
                case 'f':
                    str.push_back('\f');
                    break;
                case 't':
                    str.push_back('\t');
                    break;
                case 'n':
                    str.push_back('\n');
                    break;
                case 'r':
                    str.push_back('\r');
                    break;
                case '"':
                    str.push_back('"');
                    break;
                case '\\':
                    str.push_back('\\');
                    break;
                case 'u':{
                    std::stringstream ss;
                    for(int k = 0; k < 4; k++){
                        i++;
                        ss << std::hex << str_[i];
                    }
                    char ch;
                    ss >> ch;
                    str.push_back(ch);
                    break;
                }
            }
        }else{
            str.push_back(str_[i]);
        }
    }
    str_ = str;
}
void tiny_json::String::parseForFile(){
    std::string str;
    for(int i = 0; i < str_.size(); i++){
        switch(str_[i]){
            case '\b':
                str.push_back('\\');
                str.push_back('b');
                break;
            case '\f':
                str.push_back('\\');
                str.push_back('f');
                break;
            case '\t':
                str.push_back('\\');
                str.push_back('t');
                break;
            case '\n':
                str.push_back('\\');
                str.push_back('n');
                break;
            case '\r':
                str.push_back('\\');
                str.push_back('r');
                break;
            case '"':
                str.push_back('\\');
                str.push_back('"');
                break;
            case '\\':
                str.push_back('\\');
                str.push_back('\\');
                break;
            default:
                str.push_back(str_[i]);
                break;
        }
    }
    parsed_str_ = str;
}
bool tiny_json::String::parseable(std::string str) const {
    for(int i = 0; i < str_.size(); i++){
        if(str_[i] == '\\'){
            i++;
            switch(str_[i]){
                case 'b': case 'f': case 't':
                case 'n': case 'r': case '"':
                case '\\': break;
                case 'u':{
                    std::stringstream ss;
                    int k = 0;
                    for(; k < 4 && i < str_.size(); k++){
                        i++;
                        ss << std::hex << str_[i];
                    }
                    // 不满足 \uxxxx 格式
                    if(k != 4){
                        std::cout << "[tiny_json_Error]: 字符串 " << str_
                        << " 不能转化为 String 对象!(错误类型: Unicode 非法)" << std::endl;
                        return false;
                    }
                    break;
                }
                default:
                    std::cout << "[tiny_json_Error]: 字符串 " << str_
                    << " 不能转化为 String 对象!(错误类型: 转义字符非法)" << std::endl;
                    return false;
                    break;
            }
        }else if(str_[i] == '"'){
            std::cout << "[tiny_json_Error]: 字符串 " << str_
            << " 不能转化为 String 对象!(错误类型: 转义字符非法)" << std::endl;
            return false;
        }
    }
    return true;
}
bool tiny_json::String::parseable() const { return parseable(str_); }


/**************************
* @author   Yuan.
* @date     2022/8/23
* @brief    Boolean 类实现
***************************/

// 拷贝控制成员
tiny_json::Boolean::Boolean(const bool val){ bool_ = val; }
tiny_json::Boolean::Boolean(const std::string& val){
    if(!parseable(val)){
        std::cout << "[tiny_json_Error]: 字符串 " << val << " 不能转化为 Boolean 对象!" << std::endl;
        return;
    }
    bool_ = val == "true" ? "true" : "false";
}
tiny_json::Boolean::Boolean(const Boolean& val){ bool_ = val.get(); }
tiny_json::Boolean& tiny_json::Boolean::operator=(const Boolean& val){
    bool_ = val.get();
    return *this;
}

// 功能成员
void tiny_json::Boolean::set(const bool val){ bool_ = val; }
bool tiny_json::Boolean::get() const { return bool_; }
void tiny_json::Boolean::reset(){ bool_ = false; }

std::string tiny_json::Boolean::parse(){ return bool_ ? "true" : "false"; }
bool tiny_json::Boolean::parseable(std::string val) const {
    return (val == "true" || val == "false") ? true : false;
}
bool tiny_json::Boolean::parseable() const { return true; }
