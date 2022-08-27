
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
#include <regex>
#include <stack>

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
* @date     2022/8/28
* @brief    Object 类实现
***************************/

/**************************
* @author   Yuan.
* @date     2022/8/27
* @brief    Value 类实现
***************************/

// 拷贝控制成员
tiny_json::Value::Value(): type_(Type::kNull), val_(std::make_shared<Null>()){}
tiny_json::Value::Value(const Number& val): type_(Type::kNumber), val_(std::make_shared<Number>(val)){}
tiny_json::Value::Value(const Boolean& val): type_(Type::kBoolean), val_(std::make_shared<Boolean>(val)){}
tiny_json::Value::Value(const Null& val): type_(Type::kNull), val_(std::make_shared<Null>(val)){}
tiny_json::Value::Value(const String& val): type_(Type::kString), val_(std::make_shared<String>(val)){}
tiny_json::Value::Value(const char val[]): Value(std::string(val)){}
tiny_json::Value::Value(const std::string& val){
    using namespace reg_ex;
    if(regex_match(val, kPatternNumber) || regex_match(val, kPatternHex)){
        val_ = std::make_shared<Number>(val);
    }else if(regex_match(val, kPatternString)){
        // 去除引号
        std::string temp = val;
        temp.erase(0, 1);
        temp.erase(temp.size() - 1, 1);
        val_ = std::make_shared<String>(temp);
    }else if(regex_match(val, kPatternBool)){
        val_ = std::make_shared<Boolean>(val);
    }else if(regex_match(val, kPatternNull)){
        val_ = std::make_shared<Null>(val);
    }else if(regex_match(val, kPatternArray)){
        val_ = std::make_shared<Array>(val);        // Todo

    }else if(regex_match(val, kPatternObj)){
        val_ = std::make_shared<Object>(val);       // Todo

    }else{
        std::cout << "[tiny_json_Error]: 字符串 " << val
        << " 不能转化为 Value 对象!" << std::endl;
    }
}
tiny_json::Value::Value(const Object& val): type_(Type::kObject), val_(std::make_shared<Object>(val)){}
tiny_json::Value::Value(const Array& val): type_(Type::kArray), val_(std::make_shared<Array>(val)){}
tiny_json::Value::Value(const Value& val): type_(val.type_), val_(val.val_){}
// 没有定义移动构造函数的类可能存在 Bug ???
tiny_json::Value::Value(Value&& val) noexcept{
    type_ = val.type_;
    switch(type_){
        case Type::kNumber:
            val_ = std::make_shared<Number>(std::move(static_cast<Number&>(*val.val_)));
            break;
        case Type::kBoolean:
            val_ = std::make_shared<Boolean>(std::move(static_cast<Boolean&>(*val.val_)));
            break;
        case Type::kString:
            val_ = std::make_shared<String>(std::move(static_cast<String&>(*val.val_)));
            break;
        case Type::kNull:
            val_ = std::make_shared<Null>(std::move(static_cast<Null&>(*val.val_)));
            break;
        case Type::kObject:
            val_ = std::make_shared<Object>(std::move(static_cast<Object&>(*val.val_)));
            break;
        case Type::kArray:
            val_ = std::make_shared<Array>(std::move(static_cast<Array&>(*val.val_)));
            break;
    }
}
tiny_json::Value& tiny_json::Value::operator=(const Value& val){
    type_ = val.type_;
    val_ = val.val_;
    return *this;
}
// 没有定义移动构造函数的类可能存在 Bug ???
tiny_json::Value& tiny_json::Value::operator=(Value&& val) noexcept{
    type_ = val.type_;
    switch(type_){
        case Type::kNumber:
            val_ = std::make_shared<Number>(std::move(static_cast<Number&>(*val.val_)));
            break;
        case Type::kBoolean:
            val_ = std::make_shared<Boolean>(std::move(static_cast<Boolean&>(*val.val_)));
            break;
        case Type::kString:
            val_ = std::make_shared<String>(std::move(static_cast<String&>(*val.val_)));
            break;
        case Type::kNull:
            val_ = std::make_shared<Null>(std::move(static_cast<Null&>(*val.val_)));
            break;
        case Type::kObject:
            val_ = std::make_shared<Object>(std::move(static_cast<Object&>(*val.val_)));
            break;
        case Type::kArray:
            val_ = std::make_shared<Array>(std::move(static_cast<Array&>(*val.val_)));
            break;
    }
    return *this;
}

// 功能成员
void tiny_json::Value::set(const Number& val){
    type_ = Type::kNumber;
    val_ = std::make_shared<Number>(val);
}
void tiny_json::Value::set(const Boolean& val){
    type_ = Type::kBoolean;
    val_ = std::make_shared<Boolean>(val);
}
void tiny_json::Value::set(const Null& val){
    type_ = Type::kNull;
    val_ = std::make_shared<Null>(val);
}
void tiny_json::Value::set(const String& val){
    type_ = Type::kString;
    val_ = std::make_shared<String>(val);
}
void tiny_json::Value::set(const Object& val){
    type_ = Type::kObject;
    val_ = std::make_shared<Object>(val);
}
void tiny_json::Value::set(const Array& val){
    type_ = Type::kArray;
    val_ = std::make_shared<Array>(val);
}
tiny_json::Type tiny_json::Value::getType() const { return type_; }
tiny_json::Parseable& tiny_json::Value::get(){ return *val_; }
void tiny_json::Value::reset(){
    type_ = Type::kNull;
    val_ = std::make_shared<Null>();
}

std::string tiny_json::Value::parse(){
    switch(type_){
        case Type::kNull:
            return static_cast<Null&>(*val_).parse();
            break;
        case Type::kNumber:
            return static_cast<Number&>(*val_).parse();
            break;
        case Type::kString:
            return static_cast<String&>(*val_).parse();
            break;
        case Type::kArray:
            return static_cast<Array&>(*val_).parse();
            break;
        case Type::kBoolean:
            return static_cast<Boolean&>(*val_).parse();
            break;
        case Type::kObject:
            return static_cast<Object&>(*val_).parse();
            break;
    }
    std::cout << "[tiny_json_Error]: Value 对象错误!" << std::endl;
    return "";
}
bool tiny_json::Value::parseable(const std::string& val) const {
    switch(type_){
        case Type::kNull:
            return static_cast<Null&>(*val_).parseable(val);
            break;
        case Type::kNumber:
            return static_cast<Number&>(*val_).parseable(val);
            break;
        case Type::kString:
            return static_cast<String&>(*val_).parseable(val);
            break;
        case Type::kArray:
            return static_cast<Array&>(*val_).parseable(val);
            break;
        case Type::kBoolean:
            return static_cast<Boolean&>(*val_).parseable(val);
            break;
        case Type::kObject:
            return static_cast<Object&>(*val_).parseable(val);
            break;
    }
    std::cout << "[tiny_json_Error]: Value 对象错误!" << std::endl;
    return false;
}
bool tiny_json::Value::parseable() const {
    switch(type_){
        case Type::kNull:
            return static_cast<Null&>(*val_).parseable();
            break;
        case Type::kNumber:
            return static_cast<Number&>(*val_).parseable();
            break;
        case Type::kString:
            return static_cast<String&>(*val_).parseable();
            break;
        case Type::kArray:
            return static_cast<Array&>(*val_).parseable();
            break;
        case Type::kBoolean:
            return static_cast<Boolean&>(*val_).parseable();
            break;
        case Type::kObject:
            return static_cast<Object&>(*val_).parseable();
            break;
    }
    std::cout << "[tiny_json_Error]: Value 对象错误!" << std::endl;
    return false;
}

/**************************
* @author   Yuan.
* @date     2022/8/27
* @brief    Array 类实现
***************************/

// 拷贝控制成员
tiny_json::Array::Array(std::initializer_list<Value> il){
    for(auto beg = il.begin(); beg != il.end(); ++beg){
        this->append(*beg);
    }
}
tiny_json::Array::Array(const Array& val){
    for(int i = 0; i < val.size(); i++){
        this->append(val.arr_[i]);
    }
}
tiny_json::Array::Array(const std::string& val){
    if(parseable(val)){
        // 生成 Array
        // [..., ".[].,..", '.[].,..', ...]
        std::vector<int> indexes;
        checkQuoMark(val);
        findIndexes(val, indexes);

        if(indexes.size() == 0){
            // 特殊情况，只有一个元素
            std::string element = val;
            removeBlank(element);
            append(element);
        }else{
            // 第一个和最后一个元素特殊处理
            int size = indexes.size();
            std::string element1 = val.substr(0, indexes[0]);
            std::string element2 = val.substr(indexes[size-1] + 1, val.size() - indexes[size-1]);
            removeBlank(element1);
            removeBlank(element2);
            append(element1);
            append(element2);
            for(int i = 1; i < size; i++){
                std::string element = val.substr(indexes[i-1] + 1, indexes[i] - indexes[i-1]);
                removeBlank(element);
                append(element);
            }
        }
    }else{
        std::cout << "[tiny_json_Error]: 字符串 " << val
        << " 不能转化为 Array 对象!" << std::endl;
    }
}
tiny_json::Array::Array(const char val[]): Array(std::string(val)){}
tiny_json::Array::Array(Array&& val) noexcept { arr_ = std::move(val.arr_); }
tiny_json::Array& tiny_json::Array::operator=(const Array& val){
    for(int i = 0; i < val.size(); i++){
        this->append(val.arr_[i]);
    }
    return *this;
}
tiny_json::Array& tiny_json::Array::operator=(Array&& val) noexcept {
    arr_ = std::move(val.arr_);
    return *this;
}

// 功能成员
tiny_json::Value& tiny_json::Array::operator[](size_t index){
    return get(index);
}
tiny_json::Value& tiny_json::Array::get(size_t index){
    if(index >= 0 && index < arr_.size()){
        return arr_[index];
    }else{
        std::cout << "[tiny_json_Error]: index:  " << index
        << " 超出 Array 对象大小限制!" << std::endl;
        throw("Vector Boundary Error!");
    }
}
size_t tiny_json::Array::size() const { return arr_.size(); }
void tiny_json::Array::reset(){ arr_.clear(); }
void tiny_json::Array::append(const Value& val){ arr_.emplace_back(val); }
void tiny_json::Array::append(const std::string& val){ arr_.emplace_back(Value(val)); }
void tiny_json::Array::pop(){
    if(arr_.size() > 0){
        arr_.pop_back();
    }else{
        std::cout << "[tiny_json_Error]: Array 对象没有元素可以 pop!" << std::endl;
        throw("Vector Boundary Error!");
    }
}
void tiny_json::Array::add(size_t index, const Value& val){
    if(index >= 0 && index < arr_.size()){
        arr_.emplace(arr_.begin() + index, val);
    }else{
        std::cout << "[tiny_json_Error]: index:  " << index
        << " 超出 Array 对象大小限制!" << std::endl;
        throw("Vector Boundary Error!");
    }
}
void tiny_json::Array::del(size_t index){
    if(index >= 0 && index < arr_.size()){
        arr_.erase(arr_.begin() + index);
    }else{
        std::cout << "[tiny_json_Error]: index:  " << index
        << " 超出 Array 对象大小限制!" << std::endl;
        throw("Vector Boundary Error!");
    }
}
void tiny_json::Array::set(size_t index, const Value& val){
    if(index >= 0 && index < arr_.size()){
        arr_[index] = val;
    }else{
        std::cout << "[tiny_json_Error]: index:  " << index
        << " 超出 Array 对象大小限制!" << std::endl;
        throw("Vector Boundary Error!");
    }
}

void tiny_json::Array::checkQuoMark(const std::string& val){
    int num_d = 0, num_s = 0;
    for(int i = 0; i < val.size(); i++){
        if(i > 0 && val[i] == '"' && val[i - 1] != '\\'){
            num_d++;
        }else if(i > 0 && val[i] == '\'' && val[i - 1] != '\\'){
            num_s++;
        }
    }
    if(num_d % 2 != 0 || num_s % 2 != 0){
        std::cout << "[tiny_json_Error]: 字符串 " << val
        << " 不能转化为 Array 对象!(单双引号错误)" << std::endl;
        return;
    }
}
void tiny_json::Array::findIndexes(const std::string& val, std::vector<int>& indexes){
    bool d = false, s = false;   // 是否在双引号或单引号中
    for(int i = 0; i < val.size(); i++){
        if(!d && !s && val[i] == ','){
            // 不在单双引号内的逗号
            indexes.push_back(i);
            continue;
        }
        if(!d && !s){
            if(i > 0 && val[i] == '"' && val[i - 1] != '\\'){
                // 不是转义的双引号
                d = true;
            }else if(i > 0 && val[i] == '\'' && val[i - 1] != '\\'){
                // 不是转义的单引号
                s = true;
            }
        }else{
            if(d && i > 0 && val[i] == '"' && val[i - 1] != '\\'){
                // 在双引号中，且遇到另一个非转义双引号
                d = false;
            }else if(s && i > 0 && val[i] == '\'' && val[i - 1] != '\\'){
                // 在单引号中，且遇到另一个非转义单引号
                s = false;
            }
        }
    }
}
std::string& tiny_json::Array::removeBlank(std::string& val){
    for(int i = 0; i < val.size(); i++){
        if(val[i] == '[' || val[i] == ' '){
            val.erase(i, 1);
        }else{
            break;
        }
    }
    for(int i = val.size() - 1; i >= 0; i--){
        if(val[i] == ']' || val[i] == ' '){
            val.erase(i, 1);
        }else{
            break;
        }
    }
    return val;
}

std::string tiny_json::Array::parse(){
    std::string result;
    for(int i = 0; i < arr_.size(); i++){
        if(i == 0){
            result += "[";
            result += arr_[i].parse();
        }else if(i == arr_.size() - 1){
            result += ", ";
            result += arr_[i].parse();
            result += "]";
        }else{
            result += ", ";
            result += arr_[i].parse();
        }
    }
    return result;
}
bool tiny_json::Array::parseable(const std::string& val) const {
    for(int i = 0; i < arr_.size(); i++){
        if(!arr_[i].parseable(val)){
            return false;
        }
    }
    return true;
}
bool tiny_json::Array::parseable() const {
    for(int i = 0; i < arr_.size(); i++){
        if(!arr_[i].parseable()){
            return false;
        }
    }
    return true;
}

/**************************
* @author   Yuan.
* @date     2022/8/26
* @brief    Number 类实现
***************************/

// 拷贝控制成员
tiny_json::Number::Number(const double val, const NumberType type): num_(val), type_(type){
    if(type_ == NumberType::kDefault){
        // 未设置 type 且是整数，自动转为整数
        bool is_integer = (static_cast<long long>(val) == val);
        if(is_integer){
            type_ = NumberType::kInteger;
        }
    }
}
tiny_json::Number::Number(const char val[], const NumberType type): Number(std::string(val), type){}
tiny_json::Number::Number(const std::string& val, const NumberType type): type_(type){
    bool isHex = (val.size() >= 2 && val[0] == '0') && (val[1] == 'x' || val[1] == 'X');
    if(isHex){
        // 0x... -> Hex
        try{
            num_ = static_cast<double>(std::stoi(val, nullptr, 16));
            type_ = NumberType::kHex;
            int i = val.find('.');
            if(i >= 0){
                std::cout << "[tiny_json_Warning]: 字符串 " << val
                << " 转化为 Number 对象可能会丢失精度!" << std::endl;
            }
        }catch(std::invalid_argument){
            std::cout << "[tiny_json_Error]: 字符串 " << val
            << " 不能转化为 Number 对象!" << std::endl;
        }
    }else{
        try{
            num_ = std::stod(val);
            if(type_ == NumberType::kDefault){
                // 未设置 type 且是整数，自动转为整数
                bool is_integer = (static_cast<long long>(num_) == num_);
                if(is_integer){
                    type_ = NumberType::kInteger;
                }
            }
        }catch(std::invalid_argument){
            try{
                // -0x... or abc123
                // 无法转化成十进制，尝试十六进制
                num_ = static_cast<double>(std::stoi(val, nullptr, 16));
                type_ = NumberType::kHex;
                int i = val.find('.');
                if(i >= 0){
                    std::cout << "[tiny_json_Warning]: 字符串 " << val
                    << " 转化为 Number 对象可能会丢失精度!" << std::endl;
                }
            }catch(std::invalid_argument){
                std::cout << "[tiny_json_Error]: 字符串 " << val
                << " 不能转化为 Number 对象!" << std::endl;
            }
        }
    }
}
tiny_json::Number::Number(const Number& val): num_(val.num_), type_(val.type_)
                                            , decimal_place_(val.decimal_place_){}
tiny_json::Number& tiny_json::Number::operator=(const Number& val){
    num_ = val.num_;
    type_ = val.type_;
    decimal_place_ = val.decimal_place_;
    return *this;
}
tiny_json::Number& tiny_json::Number::operator=(const double val){
    set(val);
    return *this;
}

// 功能成员
void tiny_json::Number::set(const double val){
    num_ = val;
    bool is_integer = (static_cast<long long>(val) == val);
    switch(type_){
        case NumberType::kDefault:
            if(is_integer){
                type_ = NumberType::kInteger;
            }
            break;
        case NumberType::kFloat:
            if(is_integer){
                type_ = NumberType::kInteger;
            }
            break;
        case NumberType::kInteger:
            if(!is_integer){
                type_ = NumberType::kDefault;
            }
            break;
        case NumberType::kHex:
            if(!is_integer){
                type_ = NumberType::kDefault;
            }
            break;
    }
}
double tiny_json::Number::get() const { return num_; }
void tiny_json::Number::reset(){
    num_ = 0;
    type_ = NumberType::kDefault;
    decimal_place_ = 6;
}
bool tiny_json::Number::isHex() const { return type_ == NumberType::kHex; }
void tiny_json::Number::isLoseAccuracy(const double val){
    bool is_integer = (static_cast<long long>(val) == val);
    if(!is_integer && type_ == NumberType::kInteger){
        std::cout << "[tiny_json_Warning_Number]: 浮点数 " << val
        << " 转化为整数会丢失精度!" << std::endl;
    }else if(!is_integer && type_ == NumberType::kHex){
        std::cout << "[tiny_json_Warning_Number]: 浮点数 " << val
        << " 转化为十六进制数会丢失精度!" << std::endl;
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
            ss << integer;
            str = ss.str();
            break;
        }
        case NumberType::kFloat:{
            ss << std::setprecision(decimal_place_) << num_;
            str = ss.str();
            break;
        }
        case NumberType::kHex:{
            long long integer = static_cast<long long>(num_);
            ss << std::hex << integer;
            str = "0x" + ss.str();
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
    decimal_place_ = decimal_place;
    isLoseAccuracy(num_);
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
    for(int i = 0; i < str.size(); i++){
        if(str[i] == '\\'){
            i++;
            switch(str[i]){
                case 'b': case 'f': case 't':
                case 'n': case 'r': case '"':
                case '\\': break;
                case 'u':{
                    std::stringstream ss;
                    int k = 0;
                    for(; k < 4 && i < str.size(); k++){
                        i++;
                        ss << std::hex << str[i];
                    }
                    // 不满足 \uxxxx 格式
                    if(k != 4){
                        std::cout << "[tiny_json_Error]: 字符串 " << str
                        << " 不能转化为 String 对象!(错误类型: Unicode 非法)" << std::endl;
                        return false;
                    }
                    break;
                }
                default:
                    std::cout << "[tiny_json_Error]: 字符串 " << str
                    << " 不能转化为 String 对象!(错误类型: 转义字符非法)" << std::endl;
                    return false;
                    break;
            }
        }else if(str[i] == '"'){
            std::cout << "[tiny_json_Error]: 字符串 " << str
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
