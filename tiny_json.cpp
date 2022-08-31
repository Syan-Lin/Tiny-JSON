
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
#include <algorithm>

bool tiny_json::JSON5 = true;

/**************************
* @author   Yuan.
* @date     2022/8/30
* @brief    工具函数
***************************/

// 检查能否转化为数字
static bool parseableNumber(const std::string&);
// 检查能否转化为数组
static bool parseableArray(const std::string&);
// 检查能否转化为字符串
static bool parseableString(const std::string&);
// 检查能否转化为对象
static bool parseableObject(const std::string&);
// 检查引号和括号是否合法并找到分割 ',' 的位置
static bool checkQuoMark(const std::string&, std::vector<int>&);
// 去除空格和包裹字符
static std::string& removeBlank(std::string&);

/**************************
* @author   Yuan.
* @date     2022/8/31
* @brief    外部调用
***************************/

bool tiny_json::parseable(const std::string& str, tiny_json::Type type){
    using namespace tiny_json;
    switch(type){
        case Type::kValue:
            return parseable(str, Type::kNumber) || parseable(str, Type::kString)
                || parseable(str, Type::kBoolean) || parseable(str, Type::kNull)
                || parseable(str, Type::kObject) || parseable(str, Type::kArray);
        case Type::kNumber:
            return parseableNumber(str);
        case Type::kNull:
            return str == "null" ? true : false;
        case Type::kString:
            return parseableString(str);
        case Type::kBoolean:
            return (str == "true" || str == "false") ? true : false;
        case Type::kArray:
            return parseableArray(str);
        case Type::kObject:
            return parseableObject(str);
    }
    return false;
}
std::string tiny_json::parse(Object& obj){
    return obj.parse();
}
tiny_json::Object tiny_json::parse(const std::string& str){
    tiny_json::Object o;
    o.initFromJSON(str);
    return o;
}

/**************************
* @author   Yuan.
* @date     2022/8/30
* @brief    Object 类实现
* @test     100%
***************************/

// 拷贝控制成员
tiny_json::Object::Object(const Object& obj): kv_map_(obj.kv_map_){}
tiny_json::Object::Object(Object&& obj) noexcept { kv_map_ = std::move(obj.kv_map_); }
tiny_json::Object& tiny_json::Object::operator=(const Object& obj){
    kv_map_ = obj.kv_map_;
    return *this;
}
tiny_json::Object& tiny_json::Object::operator=(Object&& obj) noexcept {
    kv_map_ = std::move(obj.kv_map_);
    return *this;
}

// 功能成员
void tiny_json::Object::add(const Key& key, const Value& val){
    if(kv_map_.find(key) != kv_map_.end()){
        std::cout << "[tiny_json_Warning_Object]: Key: " << key
        << " 已存在于 Object 对象，将覆盖!" << std::endl;
    }
    kv_map_[key] = val;
}
void tiny_json::Object::del(const Key& key){
    if(kv_map_.find(key) != kv_map_.end()){
        kv_map_.erase(key);
    }else{
        std::cout << "[tiny_json_Warning_Object]: Key: " << key
        << " 不存在!" << std::endl;
    }
}
void tiny_json::Object::set(const Key& key, const Value& val){
    kv_map_[key] = val;
}
tiny_json::Value& tiny_json::Object::get(const Key& key){
    if(kv_map_.find(key) == kv_map_.end()){
        std::cout << "[tiny_json_Error_Object]: Key: " << key
        << " 不存在，行为未定义!" << std::endl;
    }
    return kv_map_[key];
}
bool tiny_json::Object::has(const Key& key){
    return kv_map_.find(key) != kv_map_.end();
}
void tiny_json::Object::reset(){
    kv_map_.clear();
}
size_t tiny_json::Object::size(){ return kv_map_.size(); }

std::string tiny_json::Object::parse(){
    std::string result;
    result += "{";
    for(auto e : kv_map_){
        result += "\"" + e.first + "\": ";
        result += e.second.parse() + ", ";
    }
    if(kv_map_.size() > 0){
        result.erase(result.size() - 2, 2);
    }
    result += "}";
    return result;
}
void tiny_json::Object::initFromJSON(const std::string& str){
    if(str.size() < 2 || !(str[0] == '{' && str[str.size() - 1] == '}')){
        std::cout << "[tiny_json_Error]: 字符串 " << str
        << " 不能转化为 Object 对象!" << std::endl;
        return;
    }
    if(parseable(str, Type::kObject)){
        std::string str_noblank = str;
        removeBlank(str_noblank);
        std::vector<int> indexes;
        if(!checkQuoMark(str_noblank, indexes)){
            return;
        }
        if(indexes.size() == 0){
            // 特殊情况，只有一个元素
            initKV(str_noblank);
        }else{
            // 第一个和最后一个元素特殊处理
            int size = indexes.size();
            Value v;
            std::string element1 = str_noblank.substr(0, indexes[0]);
            std::string element2 = str_noblank.substr(indexes[size-1] + 1, str_noblank.size() - indexes[size-1]);
            initKV(element1);
            for(int i = 1; i < size; i++){
                std::string element = str_noblank.substr(indexes[i-1] + 1, indexes[i] - indexes[i-1] - 1);
                initKV(element);
            }
            initKV(element2);
        }
    }else{
        std::cout << "[tiny_json_Error]: 字符串 " << str
        << " 不能转化为 Object 对象!" << std::endl;
    }
}
void tiny_json::Object::initKV(const std::string& str){
    std::string key, value;
    for(int i = 0; i < str.size(); i++){
        if(str[i] != '"' && str[i] != ':'){
            key += str[i];
        }else if(str[i] == ':'){
            value = str.substr(i + 1, str.size() - i - 1);
            break;
        }
    }
    Value v;
    v.initFromJSON(value);
    kv_map_[key] = v;
}

/**************************
* @author   Yuan.
* @date     2022/8/27
* @brief    Value 类实现
* @test     100%
***************************/

// 拷贝控制成员
tiny_json::Value::Value(): type_(Type::kNull), val_(std::make_shared<Null>()){}
tiny_json::Value::Value(const double val): type_(Type::kNumber), val_(std::make_shared<Number>(val)){}
tiny_json::Value::Value(const int val): type_(Type::kNumber), val_(std::make_shared<Number>(val)){}
tiny_json::Value::Value(const bool val): type_(Type::kBoolean), val_(std::make_shared<Boolean>(val)){}
tiny_json::Value::Value(const char str[]): Value(std::string(str)){}
tiny_json::Value::Value(const std::string& str): type_(Type::kString), val_(std::make_shared<String>(str)){}
tiny_json::Value::Value(std::string&& str) noexcept {
    type_ = Type::kString;
    val_ = std::make_shared<String>(std::move(str));
}

tiny_json::Value::Value(const Object& val): type_(Type::kObject), val_(std::make_shared<Object>(val)){}
tiny_json::Value::Value(const Array& val): type_(Type::kArray), val_(std::make_shared<Array>(val)){}
tiny_json::Value::Value(Object&& val) noexcept {
    type_ = Type::kObject;
    val_ = std::make_shared<Object>(std::move(val));
}
tiny_json::Value::Value(Array&& val) noexcept {
    type_ = Type::kArray;
    val_ = std::make_shared<Array>(std::move(val));
}
tiny_json::Value::Value(const Value& val): type_(val.type_), val_(val.val_){}
tiny_json::Value::Value(Value&& val) noexcept {
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
tiny_json::Value& tiny_json::Value::operator=(Value&& val) noexcept {
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
tiny_json::Value& tiny_json::Value::operator=(const double val){
    if(type_ == Type::kNumber){
        static_cast<Number&>(*val_).set(val);
    }else{
        type_ = Type::kNumber;
        val_ = std::make_shared<Number>(val);
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const int val){
    if(type_ == Type::kNumber){
        static_cast<Number&>(*val_).set(val);
    }else{
        type_ = Type::kNumber;
        val_ = std::make_shared<Number>(val);
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const bool val){
    if(type_ == Type::kBoolean){
        static_cast<Boolean&>(*val_).set(val);
    }else{
        type_ = Type::kBoolean;
        val_ = std::make_shared<Boolean>(val);
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const std::string& str){
    if(type_ == Type::kString){
        static_cast<String&>(*val_).set(str);
    }else{
        type_ = Type::kString;
        val_ = std::make_shared<String>(str);
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const char str[]){
    return operator=(std::string(str));
}

// 功能成员
void tiny_json::Value::set(const double val){
    if(type_ == Type::kNumber){
        static_cast<Number&>(*val_).set(val);
    }else{
        type_ = Type::kNumber;
        val_ = std::make_shared<Number>(val);
    }
}
void tiny_json::Value::set(const int val){
    if(type_ == Type::kNumber){
        static_cast<Number&>(*val_).set(val);
    }else{
        type_ = Type::kNumber;
        val_ = std::make_shared<Number>(val);
    }
}
void tiny_json::Value::set(const bool val){
    if(type_ == Type::kBoolean){
        static_cast<Boolean&>(*val_).set(val);
    }else{
        type_ = Type::kBoolean;
        val_ = std::make_shared<Boolean>(val);
    }
}
void tiny_json::Value::set(const std::string& str){
    if(type_ == Type::kString){
        static_cast<String&>(*val_).set(str);
    }else{
        type_ = Type::kString;
        val_ = std::make_shared<String>(str);
    }
}
void tiny_json::Value::set(const char str[]){
    set(std::string(str));
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
        case Type::kNumber:
            return static_cast<Number&>(*val_).parse();
        case Type::kString:
            return static_cast<String&>(*val_).parse();
        case Type::kArray:
            return static_cast<Array&>(*val_).parse();
        case Type::kBoolean:
            return static_cast<Boolean&>(*val_).parse();
        case Type::kObject:
            return static_cast<Object&>(*val_).parse();
    }
    std::cout << "[tiny_json_Error_Value]: Value 对象错误!" << std::endl;
    return "";
}
void tiny_json::Value::initFromJSON(const std::string& str){
    using namespace reg_ex;
    if(regex_match(str, kPatternNumber) || regex_match(str, kPatternHex)){
        val_ = std::make_shared<Number>();
        val_->initFromJSON(str);
        type_ = Type::kNumber;
    }else if(regex_match(str, kPatternString)){
        val_ = std::make_shared<String>();
        val_->initFromJSON(str);
        type_ = Type::kString;
    }else if(regex_match(str, kPatternBool)){
        val_ = std::make_shared<Boolean>();
        val_->initFromJSON(str);
        type_ = Type::kBoolean;
    }else if(regex_match(str, kPatternNull)){
        val_ = std::make_shared<Null>();
        val_->initFromJSON(str);
        type_ = Type::kNull;
    }else if(regex_match(str, kPatternArray)){
        val_ = std::make_shared<Array>();
        val_->initFromJSON(str);
        type_ = Type::kArray;
    }else if(regex_match(str, kPatternObj)){
        val_ = std::make_shared<Object>();       // Todo
        val_->initFromJSON(str);
        type_ = Type::kObject;
    }else{
        std::cout << "[tiny_json_Error]: 字符串 " << str
        << " 不能转化为 Value 对象!" << std::endl;
    }
}

/**************************
* @author   Yuan.
* @date     2022/8/27
* @brief    Array 类实现
* @test     100%
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
tiny_json::Value& tiny_json::Array::operator[](const size_t index){
    return get(index);
}
tiny_json::Value& tiny_json::Array::get(size_t index){
    checkIndex(index);
    return arr_[index];
}
size_t tiny_json::Array::size() const { return arr_.size(); }
void tiny_json::Array::reset(){ arr_.clear(); }
void tiny_json::Array::append(const Value& val){ arr_.emplace_back(val); }
void tiny_json::Array::append(const std::string& str){ arr_.emplace_back(Value(str)); }
void tiny_json::Array::append(const char str[]){ append(std::string(str)); }
void tiny_json::Array::append(const bool val){ arr_.emplace_back(Value(val)); }
void tiny_json::Array::append(const double val){ arr_.emplace_back(Value(val)); }
void tiny_json::Array::append(const int val){ arr_.emplace_back(Value(val)); }
void tiny_json::Array::append(){ arr_.emplace_back(Value()); }
void tiny_json::Array::add(size_t index, const Value& val){
    if(checkIndexAdd(index)){
        arr_.emplace(arr_.begin() + index, val);
    }
}
void tiny_json::Array::add(size_t index, const std::string& str){
    if(checkIndexAdd(index)){
        arr_.emplace(arr_.begin() + index, Value(str));
    }
}
void tiny_json::Array::add(size_t index, const char str[]){
    add(index, std::string(str));
}
void tiny_json::Array::add(size_t index, const bool val){
    if(checkIndexAdd(index)){
        arr_.emplace(arr_.begin() + index, Value(val));
    }
}
void tiny_json::Array::add(size_t index, const double val){
    if(checkIndexAdd(index)){
        arr_.emplace(arr_.begin() + index, Value(val));
    }
}
void tiny_json::Array::add(size_t index, const int val){
    if(checkIndexAdd(index)){
        arr_.emplace(arr_.begin() + index, Value(val));
    }
}
void tiny_json::Array::add(size_t index){
    if(checkIndexAdd(index)){
        arr_.emplace(arr_.begin() + index, Value());
    }
}
void tiny_json::Array::set(const size_t index, const Value& val){
    if(checkIndex(index)){
        arr_[index] = val;
    }
}
void tiny_json::Array::set(const size_t index, const std::string& str){
    if(checkIndex(index)){
        if(arr_[index].getType() == Type::kString){
            arr_[index].set(str);
        }else{
            arr_[index] = Value(str);
        }
    }
}
void tiny_json::Array::set(const size_t index, const char str[]){
    set(index, std::string(str));
}
void tiny_json::Array::set(const size_t index, const bool val){
    if(checkIndex(index)){
        if(arr_[index].getType() == Type::kBoolean){
            arr_[index].set(val);
        }else{
            arr_[index] = Value(val);
        }
    }
}
void tiny_json::Array::set(const size_t index, const double val){
    if(checkIndex(index)){
        if(arr_[index].getType() == Type::kNumber){
            arr_[index].set(val);
        }else{
            arr_[index] = Value(val);
        }
    }
}
void tiny_json::Array::set(const size_t index, const int val){
    if(checkIndex(index)){
        if(arr_[index].getType() == Type::kNumber){
            arr_[index].set(val);
        }else{
            arr_[index] = Value(val);
        }
    }
}
void tiny_json::Array::set(const size_t index){
    if(checkIndex(index)){
        if(arr_[index].getType() != Type::kNull){
            arr_[index] = Value();
        }
    }
}
void tiny_json::Array::pop(){
    if(checkIndex(arr_.size() - 1)){
        arr_.pop_back();
    }
}
void tiny_json::Array::del(const size_t index){
    if(checkIndex(index)){
        arr_.erase(arr_.begin() + index);
    }
}
bool tiny_json::Array::checkIndex(const size_t index){
    if(index < arr_.size()){
        return true;
    }else{
        std::cout << "[tiny_json_Error_Array]: index: " << index
        << " 数组越界! at: " << parse() << std::endl;
        return false;
    }
}
bool tiny_json::Array::checkIndexAdd(const size_t index){
    if(index <= arr_.size()){
        return true;
    }else{
        std::cout << "[tiny_json_Error_Array]: index: " << index
        << " 数组越界! at: " << parse() << std::endl;
        return false;
    }
}

std::string tiny_json::Array::parse(){
    if(arr_.size() == 0){
        return "[]";
    }else if(arr_.size() == 1){
        return "[" + arr_[0].parse() + "]";
    }
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
void tiny_json::Array::initFromJSON(const std::string& val){
    if(val.size() < 2 || !(val[0] == '[' && val[val.size() - 1] == ']')){
        std::cout << "[tiny_json_Error]: 字符串 " << val
        << " 不能转化为 Array 对象!" << std::endl;
        return;
    }
    if(parseable(val, Type::kArray)){
        // 生成 Array
        // [..., ".[].,..", '.[].,..', ...]
        std::string str = val;
        removeBlank(str);
        std::vector<int> indexes;
        if(!checkQuoMark(str, indexes)){
            return;
        }
        Value v;
        if(indexes.size() == 0){
            // 特殊情况，只有一个元素
            v.initFromJSON(str);
            append(v);
        }else{
            // 第一个和最后一个元素特殊处理
            int size = indexes.size();
            std::string element1 = str.substr(0, indexes[0]);
            std::string element2 = str.substr(indexes[size-1] + 1, str.size() - indexes[size-1]);
            v.initFromJSON(element1);
            append(v);
            for(int i = 1; i < size; i++){
                std::string element = str.substr(indexes[i-1] + 1, indexes[i] - indexes[i-1] - 1);
                v.initFromJSON(element);
                append(v);
            }
            v.initFromJSON(element2);
            append(v);
        }
    }else{
        std::cout << "[tiny_json_Error]: 字符串 " << val
        << " 不能转化为 Array 对象!" << std::endl;
    }
}

/**************************
* @author   Yuan.
* @date     2022/8/26
* @brief    Number 类实现
* @test     100%
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
tiny_json::Number::Number(const int val, const NumberType type): num_(static_cast<double>(val)), type_(type){}
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
tiny_json::NumberType tiny_json::Number::getType() const { return type_; }
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
void tiny_json::Number::parseSetting(NumberType type, size_t decimal_place){
    type_ = type;
    decimal_place_ = decimal_place;
    isLoseAccuracy(num_);
}
void tiny_json::Number::initFromJSON(const std::string& str){
    if(str.size() == 0){
        std::cout << "[tiny_json_Error]: 字符串 " << str
        << " 不能转化为 Number 对象!" << std::endl;
        return;
    }
    bool isHex = (str.size() >= 2 && str[0] == '0') && (str[1] == 'x' || str[1] == 'X');
    if(isHex){
        // 0x... -> Hex
        try{
            set(static_cast<double>(std::stoi(str, nullptr, 16)));
            type_ = NumberType::kHex;
            int i = str.find('.');
            if(i >= 0){
                std::cout << "[tiny_json_Warning]: 字符串 " << str
                << " 转化为 Number 对象可能会丢失精度!" << std::endl;
            }
        }catch(std::invalid_argument){
            std::cout << "[tiny_json_Error]: 字符串 " << str
            << " 不能转化为 Number 对象!" << std::endl;
        }
    }else{
        try{
            set(std::stod(str));
        }catch(std::invalid_argument){
            try{
                // -0x... or abc123
                // 无法转化成十进制，尝试十六进制
                set(static_cast<double>(std::stoi(str, nullptr, 16)));
                if(str[0] == '-'){
                    type_ = NumberType::kInteger;
                }else{
                    type_ = NumberType::kHex;
                }
                int i = str.find('.');
                if(i >= 0){
                    std::cout << "[tiny_json_Warning]: 字符串 " << str
                    << " 转化为 Number 对象可能会丢失精度!" << std::endl;
                }
            }catch(std::invalid_argument){
                std::cout << "[tiny_json_Error]: 字符串 " << str
                << " 不能转化为 Number 对象!" << std::endl;
            }
        }
    }
}

/**************************
* @author   Yuan.
* @date     2022/8/23
* @brief    Null 类实现
* @test     100%
***************************/

// 功能成员
std::string tiny_json::Null::parse(){ return "null"; }
void tiny_json::Null::initFromJSON(const std::string& str){
    if(!parseable(str, Type::kNull)){
        std::cout << "[tiny_json_Error]: 字符串 " << str
        << " 不能转化为 Null 对象!" << std::endl;
    }
}

/**************************
* @author   Yuan.
* @date     2022/8/24
* @brief    String 类实现
* @test     100%
* @todo     support JSON5
***************************/

// 拷贝控制成员
tiny_json::String::String(){ parse(); }
tiny_json::String::String(const std::string& str): str_(str){ parse(); }
tiny_json::String::String(const char str[]): String(std::string(str)){}
tiny_json::String::String(const String& val): str_(val.str_), parsed_str_(val.parsed_str_)
                                                , is_parsed_(val.is_parsed_){}
tiny_json::String::String(String&& val) noexcept : str_(std::move(val.str_))
                                                , parsed_str_(std::move(val.parsed_str_))
                                                , is_parsed_(val.is_parsed_){}
tiny_json::String::String(std::string&& str) noexcept : str_(std::move(str)){ parse(); }
tiny_json::String& tiny_json::String::operator=(const String& val){
    str_ = val.str_;
    parsed_str_ = val.parsed_str_;
    is_parsed_ = val.is_parsed_;
    return *this;
}
tiny_json::String& tiny_json::String::operator=(String&& val) noexcept{
    str_ = std::move(val.str_);
    parsed_str_ = std::move(val.parsed_str_);
    is_parsed_ = val.is_parsed_;
    return *this;
}
tiny_json::String& tiny_json::String::operator=(const std::string& str){
    str_ = str;
    is_parsed_ = false;
    parse();
    return *this;
}
tiny_json::String& tiny_json::String::operator=(std::string&& str) noexcept{
    str_ = std::move(str);
    is_parsed_ = false;
    parse();
    return *this;
}
tiny_json::String& tiny_json::String::operator=(const char str[]){
    str_ = std::string(str);
    is_parsed_ = false;
    parse();
    return *this;
}

// 功能成员
void tiny_json::String::set(const std::string& str){
    str_ = str;
    is_parsed_ = true;
    parseForJSON();
}
void tiny_json::String::set(const char str[]){
    str_ = std::string(str);
    is_parsed_ = true;
    parseForJSON();
}
std::string tiny_json::String::get() const { return str_; }
std::string tiny_json::String::getJSON(){ return parse(); }
void tiny_json::String::reset(){
    str_.clear();
    parsed_str_.clear();
    is_parsed_ = false;
}

std::string tiny_json::String::parse() {
    std::string str = "\"" + str_ + "\"";
    if(is_parsed_){
        return parsed_str_;
    }else if(!parseableString(str)){
        return "\"\"";
    }
    is_parsed_ = true;
    parseForPrint();
    parseForJSON();

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
void tiny_json::String::parseForJSON(){
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
    parsed_str_ = "\"" + str + "\"";
}
void tiny_json::String::initFromJSON(const std::string& str){
    if(!parseableString(str)){
        std::cout << "[tiny_json_Error]: 字符串 " << str
        << " 不能转化为 String 对象!" << std::endl;
        return;
    }
    std::string temp = str;
    // 去除引号
    temp.erase(0, 1);
    temp.erase(temp.size() - 1, 1);
    str_ = temp;
    is_parsed_ = false;
    parse();
}

/**************************
* @author   Yuan.
* @date     2022/8/23
* @brief    Boolean 类实现
* @test     100%
***************************/

// 拷贝控制成员
tiny_json::Boolean::Boolean(const bool val): bool_(val) {}
tiny_json::Boolean::Boolean(const Boolean& val): bool_(val.bool_){}
tiny_json::Boolean& tiny_json::Boolean::operator=(const Boolean& val){
    bool_ = val.bool_;
    return *this;
}
tiny_json::Boolean& tiny_json::Boolean::operator=(const bool val){
    set(val);
    return *this;
}

// 功能成员
void tiny_json::Boolean::set(const bool val){ bool_ = val; }
bool tiny_json::Boolean::get() const { return bool_; }
void tiny_json::Boolean::reset(){ bool_ = false; }

std::string tiny_json::Boolean::parse(){
    return bool_ ? "true" : "false";
}
void tiny_json::Boolean::initFromJSON(const std::string& str){
    if(!parseable(str, Type::kBoolean)){
        std::cout << "[tiny_json_Error]: 字符串 " << str << " 不能转化为 Boolean 对象!" << std::endl;
        return;
    }
    bool_ = (str == "true") ? true : false;
}

/**************************
* @author   Yuan.
* @date     2022/8/30
* @brief    工具函数实现
* @test     JSON5 单引号待测试
***************************/

static bool checkQuoMark(const std::string& str, std::vector<int>& indexes){
    // 双引号、单引号、方括号和大括号数量
    int num_d = 0, num_s = 0, f = 0, d = 0;
    for(int i = 0; i < str.size(); i++){
        if(i == 0 && str[i] == '"'){
            num_d++;
        }else if(i == 0 && str[i] == '\''){
            num_s++;
        }else if(i > 0 && num_d > 0 && str[i] == '"' && str[i-1] != '\\'){
            // 处于双引号中
            num_d--;
        }else if(i > 0 && num_s > 0 && str[i] == '\'' && str[i-1] != '\\'){
            // 处于单引号中
            num_s--;
        }

        if(num_s == 0 && num_d == 0){
            if(str[i] == '['){
                f++;
            }else if(str[i] == ']'){
                f--;
            }else if(str[i] == '{'){
                d++;
            }else if(str[i] == '}'){
                d--;
            }
        }
        if(num_d == 0 && num_s == 0 && f == 0 && d == 0 && str[i] == ','){
            indexes.push_back(i);
        }
    }
    if(d != 0 || f != 0){
        std::cout << "[tiny_json_Error]: 字符串 " << str
        << " 不能转化为 Object 或 Array 对象!(括号错误)" << std::endl;
        return false;
    }else if(num_d != 0 || num_s != 0){
        std::cout << "[tiny_json_Error]: 字符串 " << str
        << " 不能转化为 Object 或 Array 对象!(单双引号错误)" << std::endl;
        return false;
    }
    return true;
}
static std::string& removeBlank(std::string& val){
    bool doub = false, sng = false;   // 是否在双引号或单引号中
    for(int i = 0; i < val.size(); i++){
        if(!doub && !sng && (val[i] == '\n' || val[i] == '\t' || val[i] == ' ')){
            val.erase(i, 1);
            i--;
            continue;
        }
        if(!doub && !sng){
            if(i > 0 && val[i] == '"' && val[i - 1] != '\\'){
                doub = true;
            }else if(i > 0 && val[i] == '\'' && val[i - 1] != '\\'){
                sng = true;
            }
        }else{
            if(doub && i > 0 && val[i] == '"' && val[i - 1] != '\\'){
                doub = false;
            }else if(sng && i > 0 && val[i] == '\'' && val[i - 1] != '\\'){
                sng = false;
            }
        }
    }
    // 去除 "{}" 或 "[]"
    val.erase(0, 1);
    val.erase(val.size() - 1, 1);
    return val;
}
static bool parseableArray(const std::string& str){
    if(str.size() < 2){
        return false;
    }
    using namespace tiny_json;
    using namespace std;
    string str_temp = str;
    removeBlank(str_temp);
    std::vector<int> indexes;
    if(!checkQuoMark(str_temp, indexes)){
        return false;
    }

    // 检查逗号内容是否合法
    if(indexes.size() == 0){
            // 特殊情况，只有一个元素
            string element = str_temp;
            if(!parseable(element, Type::kValue)){
                return false;
            }
        }else{
            // 第一个和最后一个元素特殊处理
            int size = indexes.size();
            std::string element1 = str_temp.substr(0, indexes[0]);
            std::string element2 = str_temp.substr(indexes[size-1] + 1, str_temp.size() - indexes[size-1]);
            if(!parseable(element1, Type::kValue)){
                return false;
            }
            for(int i = 1; i < size; i++){
                std::string element = str_temp.substr(indexes[i-1] + 1, indexes[i] - indexes[i-1] - 1);
                if(!parseable(element, Type::kValue)){
                    return false;
                }
            }
            if(!parseable(element2, Type::kValue)){
                return false;
            }
        }
    return true;
}
static bool parseableNumber(const std::string& str){
    try{
        std::stod(str);
    }catch(std::invalid_argument){
        try{
            // 无法转化成十进制，尝试十六进制
            static_cast<double>(std::stoi(str, nullptr, 16));
        }catch(std::invalid_argument){
            return false;
        }
    }
    return true;
}
// 带有引号的字符串
static bool parseableString(const std::string& val){
    if(val.size() < 2 || !(val[0] == '"' && val[val.size() - 1] == '"')){
        return false;
    }
    std::string str = val;
    // 去除引号
    str.erase(0, 1);
    str.erase(str.size() - 1, 1);
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
                        return false;
                    }
                    break;
                }
                default:
                    return false;
            }
        }else if(str[i] == '"'){
            return false;
        }
    }
    return true;
}
static bool checkKV(const std::string& str){
    std::string key, value;
    for(int i = 0; i < str.size(); i++){
        if(str[i] != '"' && str[i] != ':'){
            key += str[i];
        }else if(str[i] == ':'){
            value = str.substr(i + 1, str.size() - i - 1);
            break;
        }
    }
    return parseable(value, tiny_json::Type::kValue);
}
static bool parseableObject(const std::string& str){
    if(str.size() < 2 || !(str[0] == '{' && str[str.size() - 1] == '}')){
        return false;
    }
    std::string str_noblank = str;
    removeBlank(str_noblank);
    std::vector<int> indexes;
    if(!checkQuoMark(str_noblank, indexes)){
        return false;
    }
    // 检查元素是否合法
    if(indexes.size() == 0){
        // 特殊情况，只有一个元素
        if(!checkKV(str_noblank)){
            return false;
        }
    }else{
        // 第一个和最后一个元素特殊处理
        int size = indexes.size();
        std::string element1 = str_noblank.substr(0, indexes[0]);
        std::string element2 = str_noblank.substr(indexes[size-1] + 1, str_noblank.size() - indexes[size-1]);
        if(!checkKV(element1) || !checkKV(element2)){
            return false;
        }
        for(int i = 1; i < size; i++){
            std::string element = str_noblank.substr(indexes[i-1] + 1, indexes[i] - indexes[i-1] - 1);
            if(!checkKV(element)){
                return false;
            }
        }
    }
    return true;
}
