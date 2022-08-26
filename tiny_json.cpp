
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
// tiny_json::Object tiny_json::parse(const std::string str){
//     return Object();
// }

/**************************
* @author   Yuan.
* @date     2022/8/25
* @brief    Value 类实现
***************************/

// 拷贝控制成员
tiny_json::Value::Value(): type_(Type::kNull), val_(std::make_unique<Null>()){}
tiny_json::Value::Value(const Number& val): type_(Type::kNumber), val_(std::make_unique<Number>(val)){}
tiny_json::Value::Value(const Boolean& val): type_(Type::kBoolean), val_(std::make_unique<Boolean>(val)){}
tiny_json::Value::Value(const Null& val): type_(Type::kNull), val_(std::make_unique<Null>(val)){}
tiny_json::Value::Value(const String& val): type_(Type::kString), val_(std::make_unique<String>(val)){}
//tiny_json::Value::Value(const Object& val): type_(Type::kObject), val_(std::make_unique<Object>(val)){}
//tiny_json::Value::Value(const Array& val): type_(Type::kArray), val_(std::make_unique<Array>(val)){}
//tiny_json::Value::Value(const Value& val): type_(val.getType()){ setUnion(val); }
// 没有定义移动构造函数的类可能存在 Bug
// tiny_json::Value::Value(Value&& val) noexcept{
//     type_ = val.getType();
//     switch(type_){
//         case Type::kNumber:
//             val_ = std::make_unique<Number>(std::move(val.get()));
//             break;
//         case Type::kBoolean:
//             val_ = std::make_unique<Boolean>(std::move(val.get()));
//             break;
//         case Type::kString:
//             val_ = std::make_unique<String>(std::move(val.get()));
//             break;
//         case Type::kNull:
//             val_ = std::make_unique<Null>(std::move(val.get()));
//             break;
//         case Type::kObject:
//             val_ = std::make_unique<Object>(std::move(val.get()));
//             break;
//         case Type::kArray:
//             val_ = std::make_unique<Array>(std::move(val.get()));
//             break;
//     }
// }
// tiny_json::Value& tiny_json::Value::operator=(const Value& val){
//     type_ = val.getType();
//     setUnion(val);
//     return *this;
// }
// 没有定义移动构造函数的类可能存在 Bug
// tiny_json::Value& tiny_json::Value::operator=(Value&& val) noexcept{
//     type_ = val.getType();
//     switch(type_){
//         case Type::kNumber:
//             val_ = std::make_unique<Number>(std::move(val.get()));
//             break;
//         case Type::kBoolean:
//             val_ = std::make_unique<Boolean>(std::move(val.get()));
//             break;
//         case Type::kString:
//             val_ = std::make_unique<String>(std::move(val.get()));
//             break;
//         case Type::kNull:
//             val_ = std::make_unique<Null>(std::move(val.get()));
//             break;
//         case Type::kObject:
//             val_ = std::make_unique<Object>(std::move(val.get()));
//             break;
//         case Type::kArray:
//             val_ = std::make_unique<Array>(std::move(val.get()));
//             break;
//     }
//     return *this;
// }

// 功能成员
void tiny_json::Value::set(const Number& val){
    type_ = Type::kNumber;
    val_ = std::make_unique<Number>(val);
}
void tiny_json::Value::set(const Boolean& val){
    type_ = Type::kBoolean;
    val_ = std::make_unique<Boolean>(val);
}
void tiny_json::Value::set(const Null& val){
    type_ = Type::kNull;
    val_ = std::make_unique<Null>(val);
}
void tiny_json::Value::set(const String& val){
    type_ = Type::kString;
    val_ = std::make_unique<String>(val);
}
// void tiny_json::Value::set(const Object& val){
//     type_ = Type::kObject;
//     val_ = std::make_unique<Object>(val);
// }
// void tiny_json::Value::set(const Array& val){
//     type_ = Type::kArray;
//     val_ = std::make_unique<Array>(val);
// }
// void tiny_json::Value::set(const Value& val){
//     type_ = val.getType();
//     setUnion(val);
// }
tiny_json::Type tiny_json::Value::getType() const { return type_; }
tiny_json::Parseable& tiny_json::Value::get(){ return *val_; }
void tiny_json::Value::reset(){
    type_ = Type::kNull;
    val_ = std::make_unique<Null>();
}

// void tiny_json::Value::setUnion(const Value& val){
//     switch(type_){
//         case Type::kNumber:
//             val_ = std::make_unique<Number>(val.get());
//             break;
//         case Type::kBoolean:
//             val_ = std::make_unique<Boolean>(val.get());
//             break;
//         case Type::kString:
//             val_ = std::make_unique<String>(val.get());
//             break;
//         case Type::kNull:
//             val_ = std::make_unique<Null>(val.get());
//             break;
//         case Type::kObject:
//             val_ = std::make_unique<Object>(val.get());
//             break;
//         case Type::kArray:
//             val_ = std::make_unique<Array>(val.get());
//             break;
//     }
// }

std::string tiny_json::Value::parse(){
    return "";
}
bool tiny_json::Value::parseable(const std::string& val) const {
    return true;
}
bool tiny_json::Value::parseable() const {
    return true;
}


/**************************
* @author   Yuan.
* @date     2022/8/25
* @brief    Array 类实现
***************************/
/*
// 拷贝控制成员
tiny_json::Array::Array(std::initializer_list<Value> il){
    for(auto beg = il.begin(); beg != il.end(); ++beg){
        this->append(*beg);
    }
}
tiny_json::Array::Array(const Array& val){
    // for(int i = 0; i < val.size(); i++){
    //     Value v = val.get(i);
    //     this->append(v);
    // }
}
tiny_json::Array::Array(const std::string& val){
    if(!parseable(val)){
        std::cout << "[tiny_json_Error]: 字符串 " << val
        << " 不能转化为 Array 对象!" << std::endl;
    }else{
        // 生成 Array
    }
}
tiny_json::Array::Array(const char val[]): Array(std::string(val)){}



// 功能成员
tiny_json::Value& tiny_json::Array::operator[](size_t index){ return arr_[index]; }
tiny_json::Value& tiny_json::Array::get(size_t index){ return arr_[index]; }
size_t tiny_json::Array::size() const { return arr_.size(); }
void tiny_json::Array::reset(){ arr_.clear(); }
void tiny_json::Array::append(const Value& val){ arr_.push_back(val); }
void tiny_json::Array::pop(){ arr_.pop_back(); }
void tiny_json::Array::add(size_t index, const Value& val){ arr_.insert(arr_.begin() + index, val); }
void tiny_json::Array::del(size_t index){ arr_.erase(arr_.begin() + index); }
void tiny_json::Array::set(size_t index, const Value& val){ arr_[index] = val; }


std::string tiny_json::Array::parse(){

}
bool tiny_json::Array::parseable(const std::string&) const {

}
bool tiny_json::Array::parseable() const {
    return true;
}

*/



/**************************
* @author   Yuan.
* @date     2022/8/24
* @brief    Number 类实现
***************************/

// 拷贝控制成员
tiny_json::Number::Number(const double val, bool hex): num_(val), hex_(hex){
    if(hex_){
        type_ = NumberType::kInteger;
    }
}
tiny_json::Number::Number(const char val[], bool hex): Number(std::string(val), hex){}
tiny_json::Number::Number(const std::string& val, bool hex): hex_(hex){
    bool isHex = (val.size() >= 2 && val[0] == '0') && (val[1] == 'x' || val[1] == 'X');
    if(hex_ || isHex){
        type_ = NumberType::kInteger;
        hex_ = true;
        try{
            num_ = static_cast<double>(std::stoi(val, nullptr, 16));
            int i = val.find('.');
            if(i >= 0){
                std::cout << "[tiny_json_Warning_Number]: 字符串 " << val
                << " 转化为 Number 对象可能会丢失精度!" << std::endl;
            }
        }catch(std::invalid_argument){
            std::cout << "[tiny_json_Error_Number]: 字符串 " << val
            << " 不能转化为 Number 对象!" << std::endl;
        }
    }else{
        try{
            num_ = std::stod(val);
        }catch(std::invalid_argument){
            try{
                // 无法转化成十进制，尝试十六进制
                num_ = static_cast<double>(std::stoi(val, nullptr, 16));
                hex_ = true;
                type_ = NumberType::kInteger;
                int i = val.find('.');
                if(i >= 0){
                    std::cout << "[tiny_json_Warning_Number]: 字符串 " << val
                    << " 转化为 Number 对象可能会丢失精度!" << std::endl;
                }
            }catch(std::invalid_argument){
                std::cout << "[tiny_json_Error_Number]: 字符串 " << val
                << " 不能转化为 Number 对象!" << std::endl;
            }
        }
    }
}
tiny_json::Number::Number(const Number& val): num_(val.num_), hex_(val.hex_)
                                            , type_(val.type_), decimal_place_(val.decimal_place_){}
tiny_json::Number& tiny_json::Number::operator=(const Number& val){
    num_ = val.num_;
    hex_ = val.hex_;
    type_ = val.type_;
    decimal_place_ = val.decimal_place_;
    return *this;
}
tiny_json::Number& tiny_json::Number::operator=(const double val){
    isLoseAccuracy(val);
    num_ = val;
    return *this;
}

// 功能成员
void tiny_json::Number::set(const double val){ num_ = val; isLoseAccuracy(num_); }
double tiny_json::Number::get() const { return num_; }
void tiny_json::Number::reset(){
    num_ = 0;
    type_ = NumberType::kDefault;
    hex_ = false;
    decimal_place_ = 6;
}
bool tiny_json::Number::isHex() const { return hex_; }
void tiny_json::Number::setHex(const bool hex){
    hex_ = hex;
    isLoseAccuracy(num_);
    if(hex_){
        type_ = NumberType::kInteger;
        num_ = static_cast<long long>(num_);
    }
}
void tiny_json::Number::isLoseAccuracy(const double val){
    long long integer = static_cast<long long>(val);
    bool is_integer = (integer == val);
    if(hex_ && !is_integer){
        std::cout << "[tiny_json_Warning_Number]: 浮点数 " << val
        << " 转化为十六进制数会丢失精度!" << std::endl;
    }else if(!is_integer && type_ == NumberType::kInteger){
        std::cout << "[tiny_json_Warning_Number]: 浮点数 " << val
        << " 输出为整数会丢失精度!" << std::endl;
    }
}

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
            if(hex_){
                ss << std::hex << integer;
                str = "0x" + ss.str();
            }else{
                ss << integer;
                str = ss.str();
            }
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

bool tiny_json::Number::parseable(const std::string& val) const {
    try{
        std::stod(val);
    }catch(std::invalid_argument){
        try{
            // 无法转化成十进制，尝试十六进制
            static_cast<double>(std::stoi(val, nullptr, 16));
            int i = val.find('.');
            if(i >= 0){
                std::cout << "[tiny_json_Warning]: 字符串 " << val
                << " 转化为 Number 对象可能会丢失精度!" << std::endl;
            }
        }catch(std::invalid_argument){
            return false;
        }
    }
    return true;
}
bool tiny_json::Number::parseable() const { return true; }
void tiny_json::Number::parseSetting(NumberType type, size_t decimal_place){
    type_ = type;
    if(hex_ && type_ != NumberType::kInteger){
        std::cout << "[tiny_json_Error_Number]: 十六进制数只能以整数形式输出! " << std::endl;
        type_ = NumberType::kInteger;
    }
    decimal_place_ = decimal_place;
}

/**************************
* @author   Yuan.
* @date     2022/8/23
* @brief    Null 类实现
***************************/

// 拷贝控制成员
tiny_json::Null::Null(const std::string& val){
    if(val != "null"){
        std::cout << "[tiny_json_Error]: 字符串 " << val
        << " 不能转化为 Null 对象!" << std::endl;
    }
}
tiny_json::Null::Null(const char val[]): Null(std::string(val)){}

// 功能成员
std::string tiny_json::Null::parse(){ return "null"; }
bool tiny_json::Null::parseable(const std::string& val) const { return val == "null" ? true : false; }
bool tiny_json::Null::parseable() const { return true; }

/**************************
* @author   Yuan.
* @date     2022/8/24
* @brief    String 类实现
***************************/

// 拷贝控制成员
tiny_json::String::String(const std::string& val): str_(val){ parse(); }
tiny_json::String::String(const char val[]): String(std::string(val)){}
tiny_json::String::String(const String& val): str_(val.str_), parsed_str_(val.parsed_str_){}
tiny_json::String::String(String&& val) noexcept : str_(std::move(val.str_)), parsed_str_(std::move(val.parsed_str_)){}
tiny_json::String::String(std::string&& val) noexcept : str_(std::move(val)){ parse(); }
tiny_json::String& tiny_json::String::operator=(const String& val){
    str_ = val.str_;
    parsed_str_ = val.parsed_str_;
    return *this;
}
tiny_json::String& tiny_json::String::operator=(String&& val) noexcept{
    str_ = std::move(val.str_);
    parsed_str_ = std::move(val.parsed_str_);
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
tiny_json::String& tiny_json::String::operator=(const char val[]){
    str_ = std::string(val);
    parse();
    return *this;
}

// 功能成员
void tiny_json::String::set(const std::string& val){
    str_ = val;
    is_parsed_ = true;
    parseForFile();
}
std::string tiny_json::String::get() const { return str_; }
std::string tiny_json::String::getJSON() const { return parsed_str_; }
void tiny_json::String::reset(){ str_.clear(); parsed_str_.clear(); }

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
bool tiny_json::String::parseable(const std::string& str) const {
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
tiny_json::Boolean::Boolean(const bool val): bool_(val) {}
tiny_json::Boolean::Boolean(const char val[]): Boolean(std::string(val)){}
tiny_json::Boolean::Boolean(const Boolean& val): bool_(val.bool_){}
tiny_json::Boolean::Boolean(const std::string& val){
    if(!parseable(val)){
        std::cout << "[tiny_json_Error]: 字符串 " << val << " 不能转化为 Boolean 对象!" << std::endl;
        return;
    }
    bool_ = (val == "true") ? true : false;
}
tiny_json::Boolean& tiny_json::Boolean::operator=(const Boolean& val){
    bool_ = val.bool_;
    return *this;
}

// 功能成员
void tiny_json::Boolean::set(const bool val){ bool_ = val; }
bool tiny_json::Boolean::get() const { return bool_; }
void tiny_json::Boolean::reset(){ bool_ = false; }

std::string tiny_json::Boolean::parse(){
    if(bool_){
        return std::move(std::string("true"));
    }else{
        return std::move(std::string("false"));
    }
}
bool tiny_json::Boolean::parseable(const std::string& val) const {
    return (val == "true" || val == "false") ? true : false;
}
bool tiny_json::Boolean::parseable() const { return true; }
