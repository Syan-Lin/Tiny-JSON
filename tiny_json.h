#pragma once

#include <initializer_list>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>

namespace tiny_json{

// 是否使用 JSON5 标准
// 注意事项：JSON5 下字符串请一律使用单引号包裹，且字符串中单引号需要转义
inline bool JSON5 = false;

// 类型别名定义
class Json;
using Array  = std::vector<Json>;
using Object = std::map<std::string, Json>;

enum class Type{
    NUll, ARRAY, NUMBER, STRING, OBJECT, BOOLEAN
};

class JsonValue{
public:
    virtual double             get_double() const = 0;
    virtual int                get_int()    const = 0;
    virtual bool               get_bool()   const = 0;
    virtual const std::string& get_string() const = 0;
    virtual const Array&       get_array()  const = 0;
    virtual const Object&      get_object() const = 0;
    virtual Json& operator[](const std::string&) = 0;
    virtual Json& operator[](size_t) = 0;
    virtual Type type() const = 0;
    virtual void parse(std::string& json) const = 0;
    virtual ~JsonValue() {}
};

class Json{
public:
    // 构造函数
    Json();                     // null
    Json(std::nullptr_t);       // null
    Json(double);               // number
    Json(int);                  // number
    Json(bool);                 // boolean
    Json(const std::string&);   // string copy
    Json(std::string&&);        // string move
    Json(const char*);          // string copy
    Json(const Json&);          // Json copy
    Json(Json&&);               // Json move
    Json(const Array&);         // array copy
    Json(Array&&);              // array move
    Json(const Object&);        // object copy
    Json(Object&&);             // object move
    ~Json();

    // 类型判断
    bool is_null()   const;
    bool is_number() const;
    bool is_bool()   const;
    bool is_string() const;
    bool is_array()  const;
    bool is_object() const;

    // 获取值
    double             get_double() const;
    int                get_int()    const;
    bool               get_bool()   const;
    const std::string& get_string() const;
    const Array&       get_array()  const;
    const Object&      get_object() const;

    // 运算符重载
    Json& operator[](size_t);
    Json& operator[](const std::string&);
    Json& operator=(const Json&);
    Json& operator=(Json&&);
    Json& operator=(std::nullptr_t);
    Json& operator=(double);
    Json& operator=(int);
    Json& operator=(bool);
    Json& operator=(const std::string&);
    Json& operator=(std::string&&);
    Json& operator=(const char*);
    Json& operator=(const Array&);
    Json& operator=(Array&&);
    Json& operator=(const Object&);
    Json& operator=(Object&&);

    // 特殊构造函数
    // 从实现接口 json() 的对象中构造
    template <class T, class = decltype(&T::json)>
    Json(const T& t) : Json(t.json()) {}

    // 键值列表初始化
    using kv = std::pair<std::string, Json>;
    Json(std::initializer_list<kv>);

    // map-like 对象 (std::map, std::unordered_map, etc)
    template <class M, class std::enable_if<
        std::is_constructible<std::string, decltype(std::declval<M>().begin()->first)>::value
        && std::is_constructible<Json, decltype(std::declval<M>().begin()->second)>::value,
            int>::type = 0>
    Json(const M& m) : Json(Object(m.begin(), m.end())) {}

    // vector-like 对象 (std::list, std::vector, std::set, etc)
    template <class V, class std::enable_if<
        std::is_constructible<Json, decltype(*std::declval<V>().begin())>::value,
            int>::type = 0>
    Json(const V& v) : Json(Array(v.begin(), v.end())) {}

    // 特殊接口
    void hex(bool);

    // 一般接口
    std::string parse() const;
    void reset();

private:
    Type type() const;
    JsonValue* m_val;
};

namespace{

// 工具函数
inline void parseImpl(std::nullptr_t, std::string& str) {
    str += "null";
}
inline void parseImpl(const bool val, std::string& str) {
    str += val ? "true" : "false";
}
inline void parseImpl(const double val, std::string& str) {
    str += std::to_string(val);  // 保留小数点后六位
}
inline void parseImpl(const int val, std::string& str, const bool hex) {
    if(hex){
        std::stringstream ss;
        ss << std::hex << val;
        str += "0x" + ss.str();
    }else{
        str += std::to_string(val);
    }
}
inline void parseImpl(const std::string& val, std::string& str) {
    str += JSON5 ? '\'' : '"';
    for(int i = 0; i < val.size(); i++){
        switch(val[i]){
            case '\b': str += "\\b"; break;
            case '\f': str += "\\f"; break;
            case '\t': str += "\\t"; break;
            case '\n': str += "\\n"; break;
            case '\r': str += "\\r"; break;
            case '\\': str += "\\\\"; break;
            case '"':  str += (JSON5 ? "\"" : "\\\""); break;
            case '\'': str += (JSON5 ? "\\'" : "'"); break;
            default:   str += val[i]; break;
        }
    }
    str += JSON5 ? '\'' : '"';
}
inline void parseImpl(const Array& val, std::string& str){
    bool is_first = true;
    str += '[';
    for(auto& e : val){
        if(!is_first) str += ',';
        str += e.parse();
        is_first = false;
    }
    str += ']';
}
inline void parseImpl(const Object& val, std::string& str){
    bool is_first = true;
    str += '{';
    for(auto& e : val){
        if(!is_first) str += ',';
        str += JSON5 ? e.first + ":" : '"' + e.first + "\":";
        str += e.second.parse();
        is_first = false;
    }
    str += '}';
}

// 全局变量，用于返回一些空对象
class JsonBoolean;
class JsonNull;
struct Global{
    static const std::string empty_string;
    static const Array  empty_array;
    static const Object empty_object;
    static JsonNull& json_null();
    static JsonBoolean& json_false();
    static JsonBoolean& json_true();
    static Json& json_default();
};

// 基类，定义接口及默认实现
template<Type m_type, class Value>
class JsonBasic : public JsonValue{
public:
    explicit JsonBasic(const Value& value) : m_val(value) {}
    explicit JsonBasic(Value&& value)      : m_val(std::move(value)) {}
    Type type() const override { return m_type; }
    void parse(std::string& json) const override { parseImpl(m_val, json); }

    double             get_double() const override { return 0; }
    int                get_int()    const override { return 0; }
    bool               get_bool()   const override { return false; }
    const std::string& get_string() const override { return Global::empty_string; }
    const Array&       get_array()  const override { return Global::empty_array; }
    const Object&      get_object() const override { return Global::empty_object; }

    Json& operator[](const std::string&) override { return Global::json_default(); }
    Json& operator[](size_t) override { return Global::json_default(); }
protected:
    Value m_val;
};

// 具体实现类
class JsonNumber : public JsonBasic<Type::NUMBER, double>{
public:
    explicit JsonNumber(double val) : JsonBasic(val) {}
    double get_double() const override { return m_val; }
    int    get_int()    const override { return (int)m_val; }

    void parse(std::string& json) const override {
        if(static_cast<int>(m_val) == m_val){
            parseImpl(static_cast<int>(m_val), json, is_hex);
        }else{
            parseImpl(m_val, json);
        }
    }

    JsonNumber& operator=(const double val) {
        m_val = val;
        return *this;
    }
public:
    bool is_hex = false;
};

class JsonBoolean : public JsonBasic<Type::BOOLEAN, bool>{
public:
    explicit JsonBoolean(bool val) : JsonBasic(val) {}
    bool get_bool() const override { return m_val; }
};

class JsonString : public JsonBasic<Type::STRING, std::string>{
public:
    explicit JsonString(const std::string& val) : JsonBasic(val) {}
    explicit JsonString(std::string &&val) : JsonBasic(std::move(val)) {}
    const std::string& get_string() const override { return m_val; }

    JsonString& operator=(const std::string& val) {
        m_val = val;
        return *this;
    }
    JsonString& operator=(std::string&& val) {
        m_val = std::move(val);
        return *this;
    }
};

class JsonArray : public JsonBasic<Type::ARRAY, Array>{
public:
    explicit JsonArray(const Array& val) : JsonBasic(val) {}
    explicit JsonArray(Array&& val) : JsonBasic(std::move(val)) {}
    const Array& get_array() const override { return m_val; }

    Array& get_vec() { return m_val; }

    Json& operator[](size_t index) override { return m_val.at(index); }
    JsonArray& operator=(const Array& val) {
        m_val = val;
        return *this;
    }
    JsonArray& operator=(Array&& val) {
        m_val = std::move(val);
        return *this;
    }
};

class JsonObject : public JsonBasic<Type::OBJECT, Object>{
public:
    explicit JsonObject(const Object& val) : JsonBasic(val) {}
    explicit JsonObject(Object&& val) : JsonBasic(std::move(val)) {}
    const Object& get_object() const override { return m_val; }

    Object& get_map() { return m_val; }

    Json& operator[](const std::string& key) override { return m_val[key]; }
    JsonObject& operator=(const Object& val) {
        m_val = val;
        return *this;
    }
    JsonObject& operator=(Object&& val) {
        m_val = std::move(val);
        return *this;
    }
};

class JsonNull : public JsonBasic<Type::NUll, std::nullptr_t>{
public:
    JsonNull() : JsonBasic(nullptr){}
};

} // 匿名内部命名空间

inline const std::string Global::empty_string;
inline const Array Global::empty_array;
inline const Object Global::empty_object;
inline Json& Global::json_default(){
    static Json json_def;
    return json_def;
}
inline JsonNull& Global::json_null(){
    static JsonNull json_n;
    return json_n;
}
inline JsonBoolean& Global::json_false(){
    static JsonBoolean json_f(false);
    return json_f;
}
inline JsonBoolean& Global::json_true(){
    static JsonBoolean json_t(true);
    return json_t;
}

// Json 实现
inline Json::Json()                       : m_val(&Global::json_null()) {}
inline Json::Json(std::nullptr_t)         : m_val(&Global::json_null()) {}
inline Json::Json(double val)             : m_val(new JsonNumber(val)) {}
inline Json::Json(int val)                : m_val(new JsonNumber(val)) {}
inline Json::Json(bool val)               : m_val(val ? &Global::json_true() : &Global::json_false()) {}
inline Json::Json(const std::string& val) : m_val(new JsonString(val)) {}
inline Json::Json(std::string&& val)      : m_val(new JsonString(std::move(val))) {}
inline Json::Json(const char* val)        : m_val(new JsonString(std::string(val))) {}
inline Json::Json(const Array& val)       : m_val(new JsonArray(val)) {}
inline Json::Json(Array&& val)            : m_val(new JsonArray(std::move(val))) {}
inline Json::Json(const Object& val)      : m_val(new JsonObject(val)) {}
inline Json::Json(Object&& val)           : m_val(new JsonObject(std::move(val))) {}
inline Json::Json(std::initializer_list<kv> il) {
    m_val = new JsonObject(Object());
    auto& mp = static_cast<JsonObject&>(*m_val).get_map();
    for(auto& e : il){
        mp.emplace(e.first, e.second);
    }
}
inline Json::Json(Json&& val)        : m_val(val.m_val) { val.m_val = nullptr; }
inline Json::Json(const Json& val) {
    switch(val.type()){
        case Type::OBJECT:  m_val = new JsonObject(val.m_val->get_object()); break;
        case Type::STRING:  m_val = new JsonString(val.m_val->get_string()); break;
        case Type::NUMBER:  m_val = new JsonNumber(val.m_val->get_double()); break;
        case Type::ARRAY:   m_val = new JsonArray(val.m_val->get_array()); break;
        case Type::BOOLEAN: m_val = val.m_val; break;
        case Type::NUll:    m_val = val.m_val; break;
    }
}
inline Json::~Json() {
    if(!m_val) return;
    if(type() != Type::BOOLEAN && type() != Type::NUll){
        delete m_val;
    }
}

inline bool Json::is_null()   const { return type() == Type::NUll; }
inline bool Json::is_number() const { return type() == Type::NUMBER; }
inline bool Json::is_bool()   const { return type() == Type::BOOLEAN; }
inline bool Json::is_string() const { return type() == Type::STRING; }
inline bool Json::is_array()  const { return type() == Type::ARRAY; }
inline bool Json::is_object() const { return type() == Type::OBJECT; }

inline double             Json::get_double() const { return m_val->get_double(); }
inline int                Json::get_int()    const { return m_val->get_int(); }
inline bool               Json::get_bool()   const { return m_val->get_bool(); }
inline const std::string& Json::get_string() const { return m_val->get_string(); }
inline const Array&       Json::get_array()  const { return m_val->get_array(); }
inline const Object&      Json::get_object() const { return m_val->get_object(); }

inline Json& Json::operator[](size_t index) {
    if(type() == Type::ARRAY){
        auto& vec = static_cast<JsonArray&>(*m_val).get_vec();
        while(index >= vec.size()){
            vec.push_back(Json());
        }
        return vec[index];
    }else{
        return Global::json_default();
    }
}
inline Json& Json::operator[](const std::string& key) {
    return type() == Type::OBJECT ? (*m_val)[key] : Global::json_default();
}
inline Json& Json::operator=(const Json& val) {
    switch(val.type()){
        case Type::OBJECT:  m_val = new JsonObject(val.m_val->get_object()); break;
        case Type::STRING:  m_val = new JsonString(val.m_val->get_string()); break;
        case Type::NUMBER:  m_val = new JsonNumber(val.m_val->get_double()); break;
        case Type::ARRAY:   m_val = new JsonArray(val.m_val->get_array()); break;
        case Type::BOOLEAN: m_val = val.m_val; break;
        case Type::NUll:    m_val = val.m_val; break;
    }
    return *this;
}
inline Json& Json::operator=(Json&& val) {
    m_val = val.m_val;
    val.m_val = nullptr;
    return *this;
}
inline Json& Json::operator=(std::nullptr_t) {
    if(type() != Type::NUll){
        if(type() != Type::BOOLEAN){
            delete m_val;
        }
        m_val = &Global::json_null();
    }
    return *this;
}
inline Json& Json::operator=(double val) {
    if(type() == Type::NUMBER){
        static_cast<JsonNumber&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonNumber(val);
    }
    return *this;
}
inline Json& Json::operator=(int val) {
    if(type() == Type::NUMBER){
        static_cast<JsonNumber&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonNumber(val);
    }
    return *this;
}
inline Json& Json::operator=(bool val) {
    if(type() == Type::BOOLEAN){
        m_val = val ? &Global::json_true() : &Global::json_false();
    }else{
        if(type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonBoolean(val);
    }
    return *this;
}
inline Json& Json::operator=(const std::string& val) {
    if(type() == Type::STRING){
        static_cast<JsonString&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonString(val);
    }
    return *this;
}
inline Json& Json::operator=(std::string&& val) {
    if(type() == Type::STRING){
        static_cast<JsonString&>(*m_val) = std::move(val);
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonString(std::move(val));
    }
    return *this;
}
inline Json& Json::operator=(const char* val) {
    if(type() == Type::STRING){
        static_cast<JsonString&>(*m_val) = std::string(val);
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonString(std::string(val));
    }
    return *this;
}
inline Json& Json::operator=(const Array& val) {
    if(type() == Type::ARRAY){
        static_cast<JsonArray&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonArray(val);
    }
    return *this;
}
inline Json& Json::operator=(Array&& val) {
    if(type() == Type::ARRAY){
        static_cast<JsonArray&>(*m_val) = std::move(val);
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonArray(std::move(val));
    }
    return *this;
}
inline Json& Json::operator=(const Object& val) {
    if(type() == Type::OBJECT){
        static_cast<JsonObject&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonObject(val);
    }
    return *this;
}
inline Json& Json::operator=(Object&& val) {
    if(type() == Type::OBJECT){
        static_cast<JsonObject&>(*m_val) = std::move(val);
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new JsonObject(std::move(val));
    }
    return *this;
}

inline void Json::hex(bool val) {
    if(type() == Type::NUMBER){
        static_cast<JsonNumber&>(*m_val).is_hex = val;
    }
}
inline void Json::reset() {
    if(type() != Type::BOOLEAN && type() != Type::NUll){
        delete m_val;
    }
    m_val = &Global::json_null();
}
inline Type Json::type() const { return m_val->type(); }

inline std::string Json::parse() const {
    std::string temp;
    m_val->parse(temp);
    return temp;
}

namespace json_parser{

// Json 解析类 TODO:
class JsonParser{
public:
    static void skip_annotation(int& index, const std::string& str){
        if(index >= str.size()) return;
        if(str[index] == '/'){
            index++;
            if(index < str.size()){
                if(str[index] == '/'){
                    index++;
                    while(index < str.size() && str[index++] != '\n');
                }else if(str[index] == '*'){
                    index++;
                    while(!(str[index++] == '*' && str[index++] == '/')){
                        if(index + 1 >= str.size()){
                            // TODO: 注释错误，/* 没有 */ 匹配
                            break;
                        }
                    }
                }else{
                    // TODO: 注释错误，只有一个/，后面不是/或*
                }
            }else{
                // TODO: 注释错误，只有一个/
            }
        }
    }
    static void skip_blanks(int& index, const std::string& str){
        while(index < str.size() && (str[index] == '\n'
            || str[index] == '\t' || str[index] == ' ')){
            ++index;
        }
    }
    static const char& move_next(int& index, const std::string& str){
        skip_blanks(index, str);
        skip_annotation(index, str);
        if(index >= str.size()) return str.back();
        return str[index++];
    }
    static Json parse_string(int& index, const std::string& str){
        std::string content;
        bool mode = str[index++] == '"';
        if(!mode && !JSON5){
            // 非 JSON5 不支持单引号字符串 TODO:
        }
        for(; index < str.size(); index++){
            if(str[index] == '\n'){
                // JSON5 字符串换行支持
                if(!JSON5){
                    // 非 JSON5 不支持字符串换行 TODO:
                }
                while(index < str.size() && (str[index] == '\n' || str[index] == '\t' || str[index] == ' ')){
                    index++;
                }
                index--;
            }else if(str[index] == '\\'){
                index++;
                switch(str[index]){
                    case 'b':  content += '\b'; break;
                    case 'f':  content += '\f'; break;
                    case 't':  content += '\t'; break;
                    case 'n':  content += '\n'; break;
                    case 'r':  content += '\r'; break;
                    case '\\': content += '\\'; break;
                    case '"':
                        if(!mode){
                            // '"' 不需要转义 TODO:
                        }
                        content += '"';
                        break;
                    case '\'':
                        if(mode){
                            // '\'' 不需要转义 TODO:
                        }
                        content += '\'';
                        break;
                    case 'u':{
                        index++;
                        if(str.size() - 1 - index < 4){
                            // \uxxxx 长度不够 TODO:
                            break;
                        }
                        // unicode 转 string
                        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                        char32_t uhan = strtol(str.substr(index, 4).c_str(), nullptr, 16);
                        content += converter.to_bytes(uhan);
                        index += 3;
                        break;
                    }
                    default:
                        // 转义符号错误 TODO:
                        break;
                }
            }else if((mode && str[index] == '"') || (!mode && str[index] == '\'')){
                break;
            }else{
                content += str[index];
            }
        }
        index++;
        return Json(content);
    }
    static Json parse_number(int& index, const std::string& str){
        bool hex = index + 1 < str.size() && str[index] == '0' && (str[index+1] == 'x');
        std::string temp;
        while(index < str.size() && temp.back() != ',' && temp.back() != ']' && temp.back() != '}'){
            temp += move_next(index, str);
        }
        if(temp.back() == ',' || temp.back() == ']' || temp.back() == '}'){
            temp.pop_back();
            index--;
        }

        Json number;
        if(hex){
            if(!JSON5){
                // 非 JSON5 不支持十六进制数字 TODO:
                return Json();
            }
            try{
                number = std::stoi(temp, nullptr, 16);
                number.hex(true);
            }catch(std::invalid_argument){
                // 十六进制转化失败 TODO:
            }
        }else{
            try{
                double t = std::stod(temp);
                number = t;
            }catch(std::invalid_argument){
                // 数字转化失败 TODO:
            }
        }
        return number;
    }
    static Json parse_true(int& index, const std::string& str){
        std::string temp = str.substr(index, 4);
        if(temp != "true"){
            // 错误 TODO:
        }
        index += 4;
        return Json(true);
    }
    static Json parse_false(int& index, const std::string& str){
        std::string temp = str.substr(index, 5);
        if(temp != "false"){
            // 错误 TODO:
        }
        index += 5;
        return Json(false);
    }
    static Json parse_null(int& index, const std::string& str){
        std::string temp = str.substr(index, 4);
        if(temp != "null"){
            // 错误 TODO:
        }
        index += 4;
        return Json();
    }
    static Json parse_array(int& index, const std::string& str){
        Json arr = Array();
        int count = 0;
        index++;

        while(index < str.size()){
            char ch = move_next(index, str);
            --index;
            switch(ch){
                case '0'...'9':
                case '-':  arr[count++] = parse_number(index, str); break;
                case 't':  arr[count++] = parse_true(index, str);   break;
                case 'f':  arr[count++] = parse_false(index, str);  break;
                case 'n':  arr[count++] = parse_null(index, str);   break;
                case '"':  arr[count++] = parse_string(index, str); break;
                case '{':  arr[count++] = parse_object(index, str); break;
                case '[':  arr[count++] = parse_array(index, str);  break;
                case '\'':
                    if(JSON5){
                        arr[count++] = parse_string(index, str);
                    }else{
                        // 非 JSON5 不支持单引号字符串 TODO:
                    }
                    break;
                default:
                    // 错误 TODO:
                    break;
            }
            ch = move_next(index, str);
            if(ch == ']'){
                index--;
                break;
            }else if(ch != ',' && move_next(index, str) != ','){
                // 期待一个 ',' 但是不是 TODO:
            }
        }
        index++;
        return arr;
    }
    static Json parse_object(int& index, const std::string& str){
        Json obj = Object();
        std::string key;
        index++;

        while(index < str.size()){
            char ch = move_next(index, str);
            --index;
            if(ch == '"'){
                index++;
                while(index < str.size() && index > 0 && !(str[index-1] != '\\' && str[index] == '"')){
                    key += str[index++];
                }
                index++;
                ch = move_next(index, str);
            }else if(JSON5){
                while(index < str.size() && str[index] != ' ' && str[index] != '\t'
                    && str[index] != '\n' && str[index] != ':' && str[index] != '\r'){
                    key += str[index++];
                }
                ch = move_next(index, str);
            }else{
                // 期待一个引号 TODO:
            }
            if(ch != ':'){
                // 期待一个冒号 TODO:
            }
            ch = move_next(index, str);
            switch(ch){
                case '0'...'9':
                case '-': obj[key] = parse_number(index, str); break;
                case 't': obj[key] = parse_true(index, str);   break;
                case 'f': obj[key] = parse_false(index, str);  break;
                case 'n': obj[key] = parse_null(index, str);   break;
                case '{': obj[key] = parse_object(index, str); break;
                case '[': obj[key] = parse_array(index, str);  break;
                case '"': obj[key] = parse_string(index, str); break;
                case '\'':
                    if(JSON5){
                        obj[key] = parse_string(index, str);
                    }else{
                        // 非 JSON5 不支持单引号字符串 TODO:
                    }
                    break;
                default:
                    // 错误 TODO:
                    break;
            }
            if(ch == '}') break;
            if(ch == ','){
                key.clear();
            }else if(move_next(index, str) != ','){
                // 错误,期待一个 ',' TODO:
            }else{
                key.clear();
            }
        }
        return obj;
    }
    static Json parse(const std::string& str){
        int i = 0;
        return parse_object(i, str);
    }

};

} // 匿名内部命名空间

} // tiny_json
