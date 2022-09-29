
/**************************
* @author   Yuan.
* @date     2022/9/19
* @brief    tiny_json 实现
* @version  v1.2 test
***************************/

#include "tiny_json.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <stack>
#include <algorithm>
#include <fstream>

bool tiny_json::JSON5 = false;
bool tiny_json::FORMAT_CHECK = true;

/**************************
* @author   Yuan.
* @date     2022/9/4
* @brief    工具函数
***************************/

// 判断是否在字符串中，不在字符串中执行 function2
template<typename function1, typename function2, typename function3>
static void fcnIfNotInQuote(const std::string&, int&, function1, function2, function3);
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
// 格式化 JSON
static std::string& format(std::string&);

/**************************
* @author   Yuan.
* @date     2022/9/4
* @brief    外部调用
***************************/

bool tiny_json::parseable(const std::string& str, tiny_json::Type type){
    if(!tiny_json::FORMAT_CHECK) return true;
    using namespace tiny_json;
    switch(type){
        case Type::kBoolean:
            return (str == "true" || str == "false") ? true : false;
        case Type::kNull:
            return str == "null" ? true : false;
        case Type::kNumber:
            return parseableNumber(str);
        case Type::kString:
            return parseableString(str);
        case Type::kArray:
            return parseableArray(str);
        case Type::kObject:
            return parseableObject(str);
        case Type::kValue:
            return parseable(str, Type::kNumber) || parseable(str, Type::kString)
                || parseable(str, Type::kBoolean) || parseable(str, Type::kNull)
                || parseable(str, Type::kObject) || parseable(str, Type::kArray);
    }
    return false;
}
std::string tiny_json::parse(Object& obj, bool form){
    std::string result;
    result = obj.parse();
    if(form){
        format(result);
    }
    return result;
}
tiny_json::Object tiny_json::parse(std::string& str){
    Object obj;
    if(JSON5){
        removeAnnotation(str);
    }
    obj.initFromJSON(str);
    return obj;
}
tiny_json::Object tiny_json::readFile(const std::string& path){
    std::ifstream file;
	file.open(path, std::ios::in);
	if(!file.is_open()){
        Log::error("file", "无法打开文件：" + path);
		return Object();
	}
	std::string buff, result;
	while(std::getline(file, buff)){
		result += buff + '\n';
	}
	file.close();
    return parse(result);
}
void tiny_json::writeFile(const std::string& path, const std::string& json){
    std::ofstream ofs;
	ofs.open(path, std::ios::out);
    if(!ofs.is_open()){
        Log::error("file", "文件写入失败，路径：" + path);
        return;
    }
    ofs << json;
    ofs.close();
}
void tiny_json::removeAnnotation(std::string& str){
    std::string temp;
    bool is_annotation = false, is_block_annotation = false;
    int i;
    fcnIfNotInQuote(str, i, [&]{
        if(is_annotation){
            while(str[i] != '\n'){
                i++;
            }
            i++;
            is_annotation = false;
        }else if(is_block_annotation){
            while(i + 1 < str.size()){
                if(str[i+1] == '/' && str[i] == '*'){
                    i += 2;
                    break;
                }else{
                    i++;
                }
            }
            is_block_annotation = false;
        }
    }, [&]{
        if(i > 0 && str[i] == '/' && str[i-1] == '/'){
            i -= 2;
            is_annotation = true;
        }else if(i > 0 && str[i] == '*' && str[i-1] == '/'){
            i -= 2;
            is_block_annotation = true;
        }
    }, [&]{
        if(!is_annotation && !is_block_annotation){
            temp += str[i];
        }else{
            temp.erase(temp.size() - 1, 1);
        }
    });
    str = std::move(temp);
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
tiny_json::Object::Object(std::initializer_list<kv> il){
    for(auto beg = il.begin(); beg != il.end(); ++beg){
        this->add(beg->first, beg->second);
    }
}
tiny_json::Object& tiny_json::Object::operator=(const Object& obj){
    kv_map_ = obj.kv_map_;
    return *this;
}
tiny_json::Object& tiny_json::Object::operator=(Object&& obj) noexcept {
    kv_map_ = std::move(obj.kv_map_);
    return *this;
}
tiny_json::Value& tiny_json::Object::operator[](const Key& key){
    return kv_map_[key];
}

// 功能成员
void tiny_json::Object::add(const Key& key, const Value& val){
    if(kv_map_.find(key) != kv_map_.end()){
        Log::error("Object", "Key: " + key + " 已存在于 Object 对象，将覆盖!");
    }
    kv_map_[key] = val;
}
void tiny_json::Object::remove(const Key& key){
    if(kv_map_.find(key) != kv_map_.end()){
        kv_map_.erase(key);
    }else{
        Log::error("Object", "Key: " + key + " 不存在!");
    }
}
tiny_json::Type tiny_json::Object::getType(const Key& key){
    return kv_map_[key].getType();
}
bool tiny_json::Object::has(const Key& key){
    return kv_map_.find(key) != kv_map_.end();
}
void tiny_json::Object::clear(){
    kv_map_.clear();
}
size_t tiny_json::Object::size(){ return kv_map_.size(); }
void tiny_json::Object::rehash(size_t size){ kv_map_.rehash(size); }

std::string tiny_json::Object::parse(){
    std::string result;
    result += "{";
    for(auto e : kv_map_){
        if(JSON5){
            result += e.first + ": ";
        }else{
            result += "\"" + e.first + "\": ";
        }
        result += e.second.parse() + ", ";
    }
    if(kv_map_.size() > 0){
        result.erase(result.size() - 2, 2);
    }
    result += "}";
    return result;
}
void tiny_json::Object::initFromJSON(const std::string& str){
    if(FORMAT_CHECK || parseable(str, Type::kObject)){
        std::string str_copy = str;
        removeBlank(str_copy);
        std::vector<int> divisions;
        if(!checkQuoMark(str_copy, divisions)){
            return;
        }
        if(divisions.size() == 0){
            // 特殊情况，只有一个元素
            initKV(str_copy);
        }else{
            // 第一个和最后一个元素特殊处理
            int size = divisions.size();
            std::string first_element = str_copy.substr(0, divisions[0]);
            std::string last_element = str_copy.substr(divisions[size-1] + 1, str_copy.size() - divisions[size-1]);
            initKV(first_element);
            for(int i = 1; i < size; i++){
                std::string element = str_copy.substr(divisions[i-1] + 1, divisions[i] - divisions[i-1] - 1);
                initKV(element);
            }
            // JSON5 ',' 后可能没有元素
            bool isElement = false;
            for(int i = 0; i < last_element.size(); i++){
                if(last_element[i] != '\b' && last_element[i] != '\f'
                    && last_element[i] != '\t' && last_element[i] != '\r'
                    && last_element[i] != '\n' && last_element[i] != ' '){
                    isElement = true;
                }
            }
            if(isElement){
                initKV(last_element);
            }
        }
    }else{
        Log::error("Object", "字符串 " + str + " 不能转化为 Object 对象!");
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
    kv_map_.insert({key, Value(std::string(value))});
}

/**************************
* @author   Yuan.
* @date     2022/8/27
* @brief    Value 类实现
* @test     100%
***************************/

// 拷贝控制成员
tiny_json::Value::Value(): type_(Type::kNull), null_val_(nullptr){}
tiny_json::Value::Value(Null): type_(Type::kNull), null_val_(nullptr){}
tiny_json::Value::Value(const double val): type_(Type::kNumber), num_val_(new Number(val)){}
tiny_json::Value::Value(const int val): type_(Type::kNumber), num_val_(new Number(val)){}
tiny_json::Value::Value(const bool val): type_(Type::kBoolean), bool_val_(val){}
tiny_json::Value::Value(const char str[]): type_(Type::kString), str_val_(new String(str)){}
tiny_json::Value::Value(const std::string& str): type_(Type::kString), str_val_(new String(str)){}
tiny_json::Value::Value(std::initializer_list<kv> il): type_(Type::kObject), obj_val_(new Object(il)){}
tiny_json::Value::Value(const Object& val): type_(Type::kObject), obj_val_(new Object(val)){}
tiny_json::Value::Value(const Array& val): type_(Type::kArray), arr_val_(new Array(val)){}
tiny_json::Value::Value(Object&& val) noexcept { type_ = Type::kObject; obj_val_ = new Object(val); }
tiny_json::Value::Value(Array&& val) noexcept { type_ = Type::kArray; arr_val_ = new Array(val); }
tiny_json::Value::Value(const Value& val): type_(val.type_){
    switch(type_){
        case Type::kString:     str_val_ = new String(*val.str_val_);   break;
        case Type::kArray:      arr_val_ = new Array(*val.arr_val_);    break;
        case Type::kObject:     obj_val_ = new Object(*val.obj_val_);   break;
        case Type::kNumber:     num_val_ = new Number(*val.num_val_);   break;
        case Type::kNull:       null_val_ = val.null_val_;              break;
        case Type::kBoolean:    bool_val_ = val.bool_val_;              break;
    }
}
tiny_json::Value::Value(Value&& val) noexcept {
    type_ = val.type_;
    switch(type_){
        case Type::kString:     str_val_ = val.str_val_; val.str_val_ = nullptr;    break;
        case Type::kArray:      arr_val_ = val.arr_val_; val.arr_val_ = nullptr;    break;
        case Type::kObject:     obj_val_ = val.obj_val_; val.obj_val_ = nullptr;    break;
        case Type::kNumber:     num_val_ = val.num_val_; val.num_val_ = nullptr;    break;
        case Type::kNull:       null_val_ = val.null_val_;                          break;
        case Type::kBoolean:    bool_val_ = val.bool_val_;                          break;
    }
}
tiny_json::Value::Value(std::string&& val){ initFromJSON(val); }
tiny_json::Value::~Value(){ reset(); }

tiny_json::Value& tiny_json::Value::operator=(const Value& val){
    if(&val == this){
        return *this;
    }
    reset();
    type_ = val.type_;
    switch(type_){
        case Type::kString:     str_val_ = new String(*val.str_val_);   break;
        case Type::kArray:      arr_val_ = new Array(*val.arr_val_);    break;
        case Type::kObject:     obj_val_ = new Object(*val.obj_val_);   break;
        case Type::kNumber:     num_val_ = new Number(*val.num_val_);   break;
        case Type::kNull:       null_val_ = val.null_val_;              break;
        case Type::kBoolean:    bool_val_ = val.bool_val_;              break;
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(Value&& val) noexcept {
    reset();
    type_ = val.type_;
    switch(type_){
        case Type::kString:     str_val_ = val.str_val_; val.str_val_ = nullptr;    break;
        case Type::kArray:      arr_val_ = val.arr_val_; val.arr_val_ = nullptr;    break;
        case Type::kObject:     obj_val_ = val.obj_val_; val.obj_val_ = nullptr;    break;
        case Type::kNumber:     num_val_ = val.num_val_; val.num_val_ = nullptr;    break;
        case Type::kNull:       null_val_ = val.null_val_;                          break;
        case Type::kBoolean:    bool_val_ = val.bool_val_;                          break;
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const double val){
    if(type_ == Type::kNumber){
        *num_val_ = val;
    }else{
        reset();
        type_ = Type::kNumber;
        num_val_ = new Number(val);
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const int val){
    if(type_ == Type::kNumber){
        *num_val_ = val;
    }else{
        reset();
        type_ = Type::kNumber;
        num_val_ = new Number(val);
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const bool val){
    if(type_ != Type::kBoolean){
        type_ = Type::kBoolean;
    }
    bool_val_ = val;
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const std::string& str){
    if(type_ == Type::kString){
        *str_val_ = str;
    }else{
        reset();
        type_ = Type::kString;
        str_val_ = new String(str);
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator=(const char str[]){ return operator=(std::string(str)); }
tiny_json::Value& tiny_json::Value::operator=(Null){ reset(); return *this; }
tiny_json::Value& tiny_json::Value::operator[](const std::string& key){
    if(type_ == Type::kObject){
        return (*obj_val_)[key];
    }else{
        Log::error("Value", "非 Object 类型无法使用 [key]: " + parse());
    }
    return *this;
}
tiny_json::Value& tiny_json::Value::operator[](const size_t index){
    if(type_ == Type::kArray){
        return (*arr_val_)[index];
    }else{
        Log::error("Value", "非 Array 类型无法使用 [index]: " + parse());
    }
    return *this;
}

// 功能成员
tiny_json::Type tiny_json::Value::getType() const { return type_; }
void tiny_json::Value::reset(){
    switch(type_){
        case Type::kString:     delete str_val_;    break;
        case Type::kArray:      delete arr_val_;    break;
        case Type::kObject:     delete obj_val_;    break;
        case Type::kNumber:     delete num_val_;    break;
    }
    type_ = Type::kNull;
    null_val_ = nullptr;
}
template<> double& tiny_json::Value::get<double>(){
    if(type_ != Type::kNumber){
        Log::error("Value", "返回类型不符合: " + parse());
    }
    return num_val_->getDouble();
}
template<> int& tiny_json::Value::get<int>(){
    if(type_ != Type::kNumber){
        Log::error("Value", "返回类型不符合: " + parse());
    }
    return num_val_->getInt();
}
template<> bool& tiny_json::Value::get<bool>(){
    if(type_ != Type::kBoolean){
        Log::error("Value", "返回类型不符合: " + parse());
    }
    return bool_val_;
}
template<> std::string& tiny_json::Value::get<std::string>(){
    if(type_ != Type::kString){
        Log::error("Value", "返回类型不符合: " + parse());
    }
    return str_val_->get();
}
template<> tiny_json::Array& tiny_json::Value::get<tiny_json::Array>(){
    if(type_ != Type::kArray){
        Log::error("Value", "返回类型不符合: " + parse());
    }
    return *arr_val_;
}
template<> tiny_json::Object& tiny_json::Value::get<tiny_json::Object>(){
    if(type_ != Type::kObject){
        Log::error("Value", "返回类型不符合: " + parse());
    }
    return *obj_val_;
}
void tiny_json::Value::parseSetting(NumberType type, size_t decimal_place){
    if(type_ == Type::kNumber){
        num_val_->parseSetting(type, decimal_place);
    }else{
        Log::warning("Value", parse() + " 非 Number 类型，不能使用 parseSetting()");
    }
}

std::string tiny_json::Value::parse(){
    switch(type_){
        case Type::kString:     return str_val_->parse();
        case Type::kArray:      return arr_val_->parse();
        case Type::kObject:     return obj_val_->parse();
        case Type::kNumber:     return num_val_->parse();
        case Type::kNull:       return "null";
        case Type::kBoolean:    return bool_val_ ? "true" : "false";
        default:                return "";
    }
}
void tiny_json::Value::initFromJSON(const std::string& str){
    using namespace reg_ex;
    if(regex_match(str, kPatternNumber) || regex_match(str, kPatternHex)){
        num_val_ = new Number();
        num_val_->initFromJSON(str);
        type_ = Type::kNumber;
    }else if(regex_match(str, kPatternString)){
        str_val_ = new String();
        str_val_->initFromJSON(str);
        type_ = Type::kString;
    }else if(regex_match(str, kPatternBool)){
        bool_val_ = (str == "true") ? true : false;
        type_ = Type::kBoolean;
    }else if(regex_match(str, kPatternNull)){
        null_val_ = nullptr;
        type_ = Type::kNull;
    }else if(regex_match(str, kPatternArray)){
        arr_val_ = new Array();
        arr_val_->initFromJSON(str);
        type_ = Type::kArray;
    }else if(regex_match(str, kPatternObj)){
        obj_val_ = new Object();
        obj_val_->initFromJSON(str);
        type_ = Type::kObject;
    }else{
        Log::error("Value", "字符串 " + str + " 不能转化为 Value 对象!");
    }
}

/**************************
* @author   Yuan.
* @date     2022/8/27
* @brief    Array 类实现
* @test     100%
***************************/

tiny_json::Array::Array(Vector&& val){
    arr_ = std::move(val);
}
tiny_json::Array::Array(const Array& val){
    arr_ = val.arr_;
}
tiny_json::Array::Array(Array&& val) noexcept {
    arr_ = std::move(val.arr_);
}
tiny_json::Array& tiny_json::Array::operator=(const Array& val){
    arr_ = val.arr_;
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
void tiny_json::Array::clear(){ arr_.clear(); }
void tiny_json::Array::append(const Value& val){ arr_.emplace_back(val); }
void tiny_json::Array::append(Value&& val){ arr_.emplace_back(std::move(val)); }
void tiny_json::Array::add(size_t index, const Value& val){
    if(checkIndexAdd(index)){
        arr_.emplace(arr_.begin() + index, val);
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
        Log::error("Array", "index: " + std::to_string(index) + " 数组越界! at: " + parse());
        return false;
    }
}
bool tiny_json::Array::checkIndexAdd(const size_t index){
    if(index <= arr_.size()){
        return true;
    }else{
        Log::error("Array", "index: " + std::to_string(index) + " 数组越界! at: " + parse());
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
void tiny_json::Array::initFromJSON(const std::string& str){
    if(FORMAT_CHECK || parseable(str, Type::kArray)){
        std::string str_copy = str;
        removeBlank(str_copy);
        // 生成 Array
        // [..., ".[].,..", '.[].,..', ...]
        std::vector<int> divisions;
        if(!checkQuoMark(str_copy, divisions)){
            return;
        }
        if(divisions.size() == 0){
            // 特殊情况，只有一个元素
            append(Value(std::string(str_copy)));
        }else{
            // 第一个和最后一个元素特殊处理
            int size = divisions.size();
            std::string first_element = str_copy.substr(0, divisions[0]);
            std::string last_element = str_copy.substr(divisions[size-1] + 1, str_copy.size() - divisions[size-1]);
            append(Value(std::string(first_element)));
            for(int i = 1; i < size; i++){
                std::string element = str_copy.substr(divisions[i-1] + 1, divisions[i] - divisions[i-1] - 1);
                append(Value(std::string(element)));
            }
            // JSON5 ',' 后可能没有元素
            bool isElement = false;
            for(int i = 0; i < last_element.size(); i++){
                if(last_element[i] != '\b' && last_element[i] != '\f'
                    && last_element[i] != '\t' && last_element[i] != '\r'
                    && last_element[i] != '\n' && last_element[i] != ' '){
                    isElement = true;
                }
            }
            if(isElement){
                append(Value(std::string(last_element)));
            }
        }
    }else{
        Log::error("Array", "字符串 " + str + " 不能转化为 Array 对象!");
    }
}

/**************************
* @author   Yuan.
* @date     2022/8/26
* @brief    Number 类实现
* @test     100%
***************************/

// 拷贝控制成员
tiny_json::Number::Number(): num_double_(0), type_(NumberType::kDefault){}
tiny_json::Number::Number(const Number& val): type_(val.type_){
    if(type_ == NumberType::kDefault || type_ == NumberType::kFloat){
        num_double_ = val.num_double_;
    }else{
        num_int_ = val.num_int_;
    }
}
tiny_json::Number::Number(Number&& val) noexcept : type_(std::move(val.type_)){
    if(type_ == NumberType::kDefault || type_ == NumberType::kFloat){
        num_double_ = std::move(val.num_double_);
    }else{
        num_int_ = std::move(val.num_int_);
    }
}
tiny_json::Number::Number(const double val) : num_double_(val){
    bool is_integer = (static_cast<long long>(val) == val);
    if(is_integer){
        type_ = NumberType::kInteger;
        int temp = static_cast<int>(num_double_);
        num_int_ = temp;
    }
}
tiny_json::Number::Number(const int val) : num_int_(val), type_(NumberType::kInteger){}
tiny_json::Number& tiny_json::Number::operator=(const double val){
    bool is_integer = (static_cast<long long>(val) == val);
    switch(type_){
        case NumberType::kDefault:
            if(is_integer){
                num_int_ = static_cast<int>(val);
                type_ = NumberType::kInteger;
            }else{
                num_double_ = val;
            }
            break;
        case NumberType::kFloat:
            if(is_integer){
                num_int_ = static_cast<int>(val);
                type_ = NumberType::kInteger;
            }else{
                num_double_ = val;
            }
            break;
        case NumberType::kInteger:
            if(!is_integer){
                num_double_ = val;
                type_ = NumberType::kDefault;
            }else{
                num_int_ = static_cast<int>(val);
            }
            break;
        case NumberType::kHex:
            if(!is_integer){
                num_double_ = val;
                type_ = NumberType::kDefault;
            }else{
                num_int_ = static_cast<int>(val);
            }
            break;
    }
    return *this;
}
tiny_json::Number& tiny_json::Number::operator=(const int val){
    if(type_ == NumberType::kFloat || type_ == NumberType::kDefault){
        type_ = NumberType::kInteger;
    }
    num_int_ = val;
    return *this;
}
double& tiny_json::Number::getDouble(){
    if(type_ == NumberType::kInteger || type_ == NumberType::kHex){
        double temp = static_cast<double>(num_int_);
        num_double_ = temp;
        type_ = NumberType::kDefault;
    }
    return num_double_;
}
int& tiny_json::Number::getInt(){
    if(type_ == NumberType::kFloat || type_ == NumberType::kDefault){
        bool is_integer = (static_cast<long long>(num_double_) == num_double_);
        if(!is_integer){
            Log::warning("Number", "浮点数 " + std::to_string(num_double_) + " 转化为整数会丢失精度!");
        }
        int temp = static_cast<int>(num_double_);
        num_int_ = temp;
        type_ = NumberType::kInteger;
    }
    return num_int_;
}
tiny_json::NumberType tiny_json::Number::getType() const { return type_; }
bool tiny_json::Number::isHex() const { return type_ == NumberType::kHex; }

std::string tiny_json::Number::parse(){
    std::string str;
    std::stringstream ss;

    switch(type_){
        case NumberType::kDefault:{
            ss << num_double_;
            str = ss.str();
            break;
        }
        case NumberType::kInteger:{
            long long integer = static_cast<long long>(num_int_);
            ss << integer;
            str = ss.str();
            break;
        }
        case NumberType::kFloat:{
            ss << std::setprecision(decimal_place_) << num_double_;
            str = ss.str();
            break;
        }
        case NumberType::kHex:{
            long long integer = static_cast<long long>(num_int_);
            ss << std::hex << integer;
            str = "0x" + ss.str();
            break;
        }
    }
    return str;
}
void tiny_json::Number::parseSetting(NumberType type, size_t decimal_place){
    if((type == NumberType::kInteger || type == NumberType::kHex)
        && (type_ == NumberType::kFloat || type_ == NumberType::kDefault)){
        bool is_integer = (static_cast<long long>(num_double_) == num_double_);
        if(!is_integer){
            Log::warning("Number", "浮点数 " + std::to_string(num_double_) + " 转化为整数会丢失精度!");
        }
        int temp = static_cast<int>(num_double_);
        num_int_ = temp;
    }
    type_ = type;
    decimal_place_ = decimal_place;
}
void tiny_json::Number::initFromJSON(const std::string& str){
    bool isHex = str.size() >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X');
    if(isHex){
        // 0x... -> Hex
        try{
            *this = std::stoi(str, nullptr, 16);
            type_ = NumberType::kHex;
        }catch(std::invalid_argument){
            Log::error("Number", "字符串 " + str + " 不能转化为 Number 对象!");
        }
    }else{
        try{
            *this = std::stod(str);
        }catch(std::invalid_argument){
            try{
                // -0x... or abc123
                // 无法转化成十进制，尝试十六进制
                *this = std::stoi(str, nullptr, 16);
                type_ = NumberType::kHex;
            }catch(std::invalid_argument){
                Log::error("Number", "字符串 " + str + " 不能转化为 Number 对象!");
            }
        }
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
tiny_json::String::String(const std::string& str): str_(str){}
tiny_json::String::String(const String& val): str_(val.str_){}
tiny_json::String::String(String&& val) noexcept : str_(std::move(val.str_)){}
tiny_json::String::String(const char str[]): String(std::string(str)){}
tiny_json::String::String(std::string&& str) noexcept : str_(std::move(str)){ parse(); }
tiny_json::String& tiny_json::String::operator=(const std::string& str){
    str_ = str;
    return *this;
}
tiny_json::String& tiny_json::String::operator=(std::string&& str) noexcept{
    str_ = std::move(str);
    return *this;
}
tiny_json::String& tiny_json::String::operator=(const char str[]){
    str_ = std::string(str);
    return *this;
}

std::string& tiny_json::String::get(){
    return str_;
}

std::string tiny_json::String::parse() {
    std::string str;
    for(int i = 0; i < str_.size(); i++){
        switch(str_[i]){
            case '\b':  str += "\\b";   break;
            case '\f':  str += "\\f";   break;
            case '\t':  str += "\\t";   break;
            case '\n':  str += "\\n";   break;
            case '\r':  str += "\\r";   break;
            case '\\':  str += "\\\\";  break;
            default:    str += str_[i]; break;
            case '"':
                if(JSON5){
                    str += '"';
                }else{
                    str += "\\\"";
                }
                break;
            case '\'':
                if(JSON5){
                    str += "\\'";
                }else{
                    str += '\'';
                }
                break;
        }
    }
    if(JSON5){
        str = '\'' + str + '\'';
    }else{
        str = '"' + str + '"';
    }
    return str;
}
void tiny_json::String::initFromJSON(const std::string& str){
    if(!FORMAT_CHECK && !parseableString(str)){
        Log::error("String", "字符串 " + str + " 不能转化为 String 对象!");
        return;
    }
    std::string str_copy;
    // 去除引号，并 JSON5 删除换行等等字符
    for(int i = 1; i < str.size() - 1; i++){
        if(str[i] == '\n'){
            while(i < str.size() - 1 && (str[i] == '\b' || str[i] == '\f'
                    || str[i] == '\t' || str[i] == '\r'
                    || str[i] == '\n' || str[i] == ' ')){
                i++;
            }
        }
        str_copy += str[i];
    }
    str_ = str_copy;
    str_copy.clear();
    for(int i = 0; i < str_.size(); i++){
        if(str_[i] == '\\'){
            i++;
            switch(str_[i]){
                case 'b':   str_copy += '\b';   break;
                case 'f':   str_copy += '\f';   break;
                case 't':   str_copy += '\t';   break;
                case 'n':   str_copy += '\n';   break;
                case 'r':   str_copy += '\r';   break;
                case '"':   str_copy += '"';    break;
                case '\'':  str_copy += '\'';   break;
                case '\\':  str_copy += '\\';   break;
                case 'u':{
                    std::stringstream ss;
                    for(int k = 0; k < 4; k++){
                        i++;
                        ss << std::hex << str_[i];
                    }
                    char ch;
                    ss >> ch;
                    str_copy += ch;
                    break;
                }
            }
        }else{
            str_copy += str_[i];
        }
    }
    str_ = str_copy;
}

/**************************
* @author   Yuan.
* @date     2022/8/30
* @brief    工具函数实现
***************************/

static bool checkQuoMark(const std::string& str, std::vector<int>& indexes){
    // 双引号、单引号、方括号和大括号数量
    int num_square = 0, num_brace = 0, i;
    fcnIfNotInQuote(str, i, []{}, [&]{
        if(str[i] == '['){
            num_square++;
        }else if(str[i] == ']'){
            num_square--;
        }else if(str[i] == '{'){
            num_brace++;
        }else if(str[i] == '}'){
            num_brace--;
        }else if(str[i] == ',' && num_square == 0 && num_brace == 0){
            indexes.push_back(i);
        }
    }, []{});
    if(num_brace + num_square > 0){
        tiny_json_log::Log::error("check", "字符串 " + str + "不能转化为 Object 或 Array 对象!(括号错误)");
        return false;
    }
    return true;
}
static std::string& removeBlank(std::string& str){
    std::string temp;
    int i;
    bool isBlank = false;
    fcnIfNotInQuote(str, i, []{}, [&]{
        while(i < str.size() && str[i] == '\n' || str[i] == '\t' || str[i] == ' '){
            isBlank = true;
            i++;
        }
    }, [&]{
        if(i > 0 && i < str.size() - 1){
            if(isBlank){
                i--;
                isBlank = false;
            }else{
                temp += str[i];
            }
        }
    });
    str = std::move(temp);
    return str;
}
static bool parseableArray(const std::string& str){
    if(str.size() < 2){
        return false;
    }
    using namespace tiny_json;
    using namespace std;
    string str_copy = str;
    removeBlank(str_copy);
    std::vector<int> indexes;
    if(!checkQuoMark(str_copy, indexes)){
        return false;
    }

    if(indexes.size() == 0){
            // 特殊情况，只有一个元素
            if(!parseable(str_copy, Type::kValue)){
                return false;
            }
        }else{
            // 第一个和最后一个元素特殊处理
            int size = indexes.size();
            std::string first_element = str_copy.substr(0, indexes[0]);
            std::string last_element = str_copy.substr(indexes[size-1] + 1, str_copy.size() - indexes[size-1]);
            if(!parseable(first_element, Type::kValue)){
                return false;
            }
            for(int i = 1; i < size; i++){
                std::string element = str_copy.substr(indexes[i-1] + 1, indexes[i] - indexes[i-1] - 1);
                if(!parseable(element, Type::kValue)){
                    return false;
                }
            }
            bool isElement = false;
            for(int i = 0; i < last_element.size(); i++){
                if(last_element[i] != '\b' && last_element[i] != '\f'
                    && last_element[i] != '\t' && last_element[i] != '\r'
                    && last_element[i] != '\n' && last_element[i] != ' '){
                    isElement = true;
                }
            }
            if(isElement && !parseable(last_element, Type::kValue)){
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
            std::stoi(str, nullptr, 16);
        }catch(std::invalid_argument){
            return false;
        }
    }
    return true;
}
// 带有引号的字符串(支持 JSON5)
static bool parseableString(const std::string& str){
    using tiny_json::JSON5;
    if(str.size() < 2){
        return false;
    }else if(!(str[0] == '"' && str[str.size()-1] == '"' && !JSON5) &&
            !(str[0] == '\'' && str[str.size()-1] == '\'' && JSON5)){
        return false;
    }
    // 去除引号
    std::string str_copy(str.begin() + 1, str.end() - 1);
    for(int i = 0; i < str_copy.size(); i++){
        if(str_copy[i] == '\\'){
            i++;
            switch(str_copy[i]){
                case 'b': case 'f': case 't':
                case 'n': case 'r': case '\\': break;
                case '"':{
                    if(JSON5){
                        return false;
                    }
                    break;
                }
                case '\'':{
                    if(!JSON5){
                        return false;
                    }
                    break;
                }
                case 'u':{
                    std::stringstream ss;
                    int k = 0;
                    for(; k < 4 && i < str_copy.size(); k++){
                        i++;
                        ss << std::hex << str_copy[i];
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
        }else if(str_copy[i] == '"' && !JSON5){
            return false;
        }else if(str_copy[i] == '\'' && JSON5){
            return false;
        }
    }
    return true;
}
static bool checkKV(const std::string& str){
    std::string key, value;
    for(int i = 0; i < str.size(); i++){
        if(str[i] == ':'){
            value = str.substr(i + 1, str.size() - i - 1);
            break;
        }
    }
    return parseable(value, tiny_json::Type::kValue);
}
static bool parseableObject(const std::string& str){
    std::string str_copy = str;
    removeBlank(str_copy);
    if(str_copy.size() < 2){
        return false;
    }
    std::vector<int> divisions;
    if(!checkQuoMark(str_copy, divisions)){
        return false;
    }
    if(divisions.size() == 0){
        // 特殊情况，只有一个元素
        if(!checkKV(str_copy)){
            return false;
        }
    }else{
        // 第一个和最后一个元素特殊处理
        int size = divisions.size();
        std::string first_element = str_copy.substr(0, divisions[0]);
        std::string last_element = str_copy.substr(divisions[size-1] + 1, str_copy.size() - divisions[size-1]);
        for(int i = 1; i < size; i++){
            std::string element = str_copy.substr(divisions[i-1] + 1, divisions[i] - divisions[i-1] - 1);
            if(!checkKV(element)){
                return false;
            }
        }
        // JSON5 ',' 后可能没有元素
        bool isElement = false;
        for(int i = 0; i < last_element.size(); i++){
            if(last_element[i] != '\b' && last_element[i] != '\f'
                && last_element[i] != '\t' && last_element[i] != '\r'
                && last_element[i] != '\n' && last_element[i] != ' '){
                isElement = true;
            }
        }
        if(!checkKV(first_element) || (isElement && !checkKV(last_element))){
            return false;
        }
    }
    return true;
}
static std::string& format(std::string& str){
    std::string json;
    int level = 0;
    auto printBlank = [&level, &json]{
        json += '\n';
        for(int j = 0; j < level; j++){
            json += '\t';
        }
    };

    bool in_double = false, in_single = false;
    for(int i = 0; i < str.size(); i++){
        json += str[i];
        if(!in_double && !in_single){
            if(str[i] == '"'){
                in_double = true;
            }else if(str[i] == '\''){
                in_single = true;
            }
            switch(str[i]){
                case '{':
                    level++;
                    printBlank();
                    break;
                case ',':
                    i++;
                    printBlank();
                    break;
                case '[':
                    level++;
                    printBlank();
                    break;
            }
        }else if(in_double && str[i] == '"' && str[i-1] != '\\'){
            in_double = false;
        }else if(in_single && str[i] == '\'' && str[i-1] != '\\'){
            in_single = false;
        }
        if(!in_double && !in_single){
            if(i + 1 < str.size() && (str[i+1] == '}' || str[i+1] == ']')){
                level--;
                printBlank();
            }
        }
    }
    str = json;
    return str;
}

template<typename function1, typename function2, typename function3>
static void fcnIfNotInQuote(const std::string& str, int& i, function1 fcn1, function2 fcn2, function3 fcn3){
    bool in_double = false, in_single = false;
    for(i = 0; i < str.size(); i++){
        fcn1();
        if(!in_double && !in_single){
            if(str[i] == '"'){
                in_double = true;
            }else if(str[i] == '\''){
                in_single = true;
            }
            fcn2();
        }else if(in_double && str[i] == '"' && str[i-1] != '\\'){
            in_double = false;
        }else if(in_single && str[i] == '\'' && str[i-1] != '\\'){
            in_single = false;
        }
        fcn3();
    }
}
