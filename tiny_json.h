#pragma once

#include <initializer_list>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>

// Console color settings
#ifdef _WIN32
#include "Windows.h"
#endif
#ifdef linux
#define RED "\033[0;32;31m"
#define NONE "\033[m"
#endif

namespace tiny_json{

struct Parameter{
    static bool JSON5;
    static bool detail;
};
bool Parameter::JSON5 = false;
bool Parameter::detail = true;
// Whether to use JSON5 standard
#define JSON5 Parameter::JSON5
// Whether to show error information
#define detail Parameter::detail

class Json;
using Array  = std::vector<Json>;
using Object = std::map<std::string, Json>;

enum class Type{
    NUll, ARRAY, NUMBER, STRING, OBJECT, BOOLEAN
};

class JsonValue{
public:
    virtual double             getDouble() const = 0;
    virtual int                getInt()    const = 0;
    virtual bool               getBool()   const = 0;
    virtual const std::string& getString() const = 0;
    virtual const Array&       getArray()  const = 0;
    virtual const Object&      getObject() const = 0;
    virtual Json& operator[](const std::string&) = 0;
    virtual Json& operator[](size_t) = 0;
    virtual Type type() const = 0;
    virtual void parse(std::string& json) const = 0;
    virtual ~JsonValue() {}
};

class Json{
public:
    // Construct functions
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

    // Type judgement
    bool isNull()   const;
    bool isNumber() const;
    bool isBool()   const;
    bool isString() const;
    bool isArray()  const;
    bool isObject() const;

    // Get value
    double             getDouble() const;
    int                getInt()    const;
    bool               getBool()   const;
    const std::string& getString() const;
    const Array&       getArray()  const;
    const Object&      getObject() const;
    std::vector<Json>&           getVec();
    std::map<std::string, Json>& getMap();

    // Operator overload
    Json& operator[](size_t);
    Json& operator[](const std::string);
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

    // Spectial construct functions
    // Construct a json object from interface json()
    template <class T, class = decltype(&T::json)>
    Json(T& t) : Json(t.json()) {}

    // key-value initializer list
    using kv = std::pair<std::string, Json>;
    Json(std::initializer_list<kv>);

    // map-like objects (std::map, std::unordered_map, etc)
    template <class M, typename std::enable_if<
        std::is_constructible<std::string, decltype(std::declval<M>().begin()->first)>::value
        && std::is_constructible<Json, decltype(std::declval<M>().begin()->second)>::value,
            int>::type = 0>
    Json(const M& m) : Json(Object(m.begin(), m.end())) {}

    // vector-like objects (std::list, std::vector, std::set, etc)
    template <class V, typename std::enable_if<
        std::is_constructible<Json, decltype(*std::declval<V>().begin())>::value,
            int>::type = 0>
    Json(const V& v) : Json(Array(v.begin(), v.end())) {}

    // Only number valid, output in hexadecimal format
    void hex(bool);

    // Parse to json string
    std::string parse() const;
    // Reset to null
    void reset();

private:
    Type type() const;
    JsonValue* m_val;
};

namespace internal_class{

// Utility functions
inline void parseImpl(std::nullptr_t, std::string& str){
    str += "null";
}
inline void parseImpl(const bool val, std::string& str){
    str += val ? "true" : "false";
}
inline void parseImpl(const double val, std::string& str){
    str += std::to_string(val);  // Keep six decimal places
}
inline void parseImpl(const int val, std::string& str, const bool hex){
    if(hex){
        std::stringstream ss;
        ss << std::hex << val;
        str += "0x" + ss.str();
    }else{
        str += std::to_string(val);
    }
}
inline void parseImpl(const std::string& val, std::string& str){
    str += JSON5 ? '\'' : '"';
    for(size_t i = 0; i < val.size(); i++){
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

// Global variables, use to return some empty values
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

// Base class, define interface and default implementation
template<Type m_type, class Value>
class JsonBasic : public JsonValue{
public:
    explicit JsonBasic(const Value& value) : m_val(value) {}
    explicit JsonBasic(Value&& value)      : m_val(std::move(value)) {}
    Type type() const override { return m_type; }
    void parse(std::string& json) const override { parseImpl(m_val, json); }

    double             getDouble() const override { return 0; }
    int                getInt()    const override { return 0; }
    bool               getBool()   const override { return false; }
    const std::string& getString() const override { return Global::empty_string; }
    const Array&       getArray()  const override { return Global::empty_array; }
    const Object&      getObject() const override { return Global::empty_object; }

    Json& operator[](const std::string&) override { return Global::json_default(); }
    Json& operator[](size_t) override { return Global::json_default(); }
protected:
    Value m_val;
};

// Concrete implementation
class JsonNumber : public JsonBasic<Type::NUMBER, double>{
public:
    explicit JsonNumber(double val) : JsonBasic(val) {}
    double getDouble() const override { return m_val; }
    int    getInt()    const override { return (int)m_val; }

    void parse(std::string& json) const override {
        if(static_cast<int>(m_val) == m_val){
            parseImpl(static_cast<int>(m_val), json, hex);
        }else{
            parseImpl(m_val, json);
        }
    }

    JsonNumber& operator=(const double val){
        m_val = val;
        return *this;
    }
public:
    bool hex = false;
};

class JsonBoolean : public JsonBasic<Type::BOOLEAN, bool>{
public:
    explicit JsonBoolean(bool val) : JsonBasic(val) {}
    bool getBool() const override { return m_val; }
};

class JsonString : public JsonBasic<Type::STRING, std::string>{
public:
    explicit JsonString(const std::string& val) : JsonBasic(val) {}
    explicit JsonString(std::string &&val) : JsonBasic(std::move(val)) {}
    const std::string& getString() const override { return m_val; }

    JsonString& operator=(const std::string& val){
        m_val = val;
        return *this;
    }
    JsonString& operator=(std::string&& val){
        m_val = std::move(val);
        return *this;
    }
};

class JsonArray : public JsonBasic<Type::ARRAY, Array>{
public:
    explicit JsonArray(const Array& val) : JsonBasic(val) {}
    explicit JsonArray(Array&& val) : JsonBasic(std::move(val)) {}
    const Array& getArray() const override { return m_val; }

    Array& getVec() { return m_val; }

    Json& operator[](size_t index) override { return m_val.at(index); }
    JsonArray& operator=(const Array& val){
        m_val = val;
        return *this;
    }
    JsonArray& operator=(Array&& val){
        m_val = std::move(val);
        return *this;
    }
};

class JsonObject : public JsonBasic<Type::OBJECT, Object>{
public:
    explicit JsonObject(const Object& val) : JsonBasic(val) {}
    explicit JsonObject(Object&& val) : JsonBasic(std::move(val)) {}
    const Object& getObject() const override { return m_val; }

    Object& getMap() { return m_val; }

    Json& operator[](const std::string& key) override { return m_val[key]; }
    JsonObject& operator=(const Object& val){
        m_val = val;
        return *this;
    }
    JsonObject& operator=(Object&& val){
        m_val = std::move(val);
        return *this;
    }
};

class JsonNull : public JsonBasic<Type::NUll, std::nullptr_t>{
public:
    JsonNull() : JsonBasic(nullptr) {}
};

const std::string Global::empty_string;
const Array Global::empty_array;
const Object Global::empty_object;
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

} // internal_class

// Json implementation
inline Json::Json()                       : m_val(&internal_class::Global::json_null()) {}
inline Json::Json(std::nullptr_t)         : m_val(&internal_class::Global::json_null()) {}
inline Json::Json(double val)             : m_val(new internal_class::JsonNumber(val)) {}
inline Json::Json(int val)                : m_val(new internal_class::JsonNumber(val)) {}
inline Json::Json(bool val)               : m_val(val ? &internal_class::Global::json_true() : &internal_class::Global::json_false()) {}
inline Json::Json(const std::string& val) : m_val(new internal_class::JsonString(val)) {}
inline Json::Json(std::string&& val)      : m_val(new internal_class::JsonString(std::move(val))) {}
inline Json::Json(const char* val)        : m_val(new internal_class::JsonString(std::string(val))) {}
inline Json::Json(const Array& val)       : m_val(new internal_class::JsonArray(val)) {}
inline Json::Json(Array&& val)            : m_val(new internal_class::JsonArray(std::move(val))) {}
inline Json::Json(const Object& val)      : m_val(new internal_class::JsonObject(val)) {}
inline Json::Json(Object&& val)           : m_val(new internal_class::JsonObject(std::move(val))) {}
inline Json::Json(std::initializer_list<kv> il){
    m_val = new internal_class::JsonObject(Object());
    auto& mp = static_cast<internal_class::JsonObject&>(*m_val).getMap();
    for(auto& e : il){
        mp.emplace(e.first, e.second);
    }
}
inline Json::Json(Json&& val)        : m_val(val.m_val) { val.m_val = nullptr; }
inline Json::Json(const Json& val){
    switch(val.type()){
        case Type::OBJECT:  m_val = new internal_class::JsonObject(val.m_val->getObject()); break;
        case Type::STRING:  m_val = new internal_class::JsonString(val.m_val->getString()); break;
        case Type::NUMBER:  m_val = new internal_class::JsonNumber(val.m_val->getDouble()); break;
        case Type::ARRAY:   m_val = new internal_class::JsonArray(val.m_val->getArray()); break;
        case Type::BOOLEAN: m_val = val.m_val; break;
        case Type::NUll:    m_val = val.m_val; break;
    }
}
inline Json::~Json(){
    if(!m_val) return;
    if(type() != Type::BOOLEAN && type() != Type::NUll){
        delete m_val;
    }
}

inline bool Json::isNull()   const { return type() == Type::NUll; }
inline bool Json::isNumber() const { return type() == Type::NUMBER; }
inline bool Json::isBool()   const { return type() == Type::BOOLEAN; }
inline bool Json::isString() const { return type() == Type::STRING; }
inline bool Json::isArray()  const { return type() == Type::ARRAY; }
inline bool Json::isObject() const { return type() == Type::OBJECT; }

inline double             Json::getDouble() const { return m_val->getDouble(); }
inline int                Json::getInt()    const { return m_val->getInt(); }
inline bool               Json::getBool()   const { return m_val->getBool(); }
inline const std::string& Json::getString() const { return m_val->getString(); }
inline const Array&       Json::getArray()  const { return m_val->getArray(); }
inline const Object&      Json::getObject() const { return m_val->getObject(); }
inline std::vector<Json>& Json::getVec(){
    if(type() != Type::ARRAY){
        throw std::runtime_error("getVec() error: type is not array");
    }
    return static_cast<internal_class::JsonArray&>(*m_val).getVec();
}
inline std::map<std::string, Json>& Json::getMap(){
    if(type() != Type::OBJECT){
        throw std::runtime_error("getMap() error: type is not object");
    }
    return static_cast<internal_class::JsonObject&>(*m_val).getMap();
}

inline Json& Json::operator[](size_t index){
    if(type() == Type::NUll){
        m_val = new internal_class::JsonArray(Array());
    }
    if(type() == Type::ARRAY){
        auto& vec = static_cast<internal_class::JsonArray&>(*m_val).getVec();
        while(index >= vec.size()){
            vec.push_back(Json());
        }
        return vec[index];
    }else{
        return internal_class::Global::json_default();
    }
}
inline Json& Json::operator[](const std::string key){
    if(type() == Type::NUll){
        m_val = new internal_class::JsonObject(Object());
    }
    return type() == Type::OBJECT ? (*m_val)[key] : internal_class::Global::json_default();
}
inline Json& Json::operator=(const Json& val){
    switch(val.type()){
        case Type::OBJECT:  m_val = new internal_class::JsonObject(val.m_val->getObject()); break;
        case Type::STRING:  m_val = new internal_class::JsonString(val.m_val->getString()); break;
        case Type::NUMBER:  m_val = new internal_class::JsonNumber(val.m_val->getDouble()); break;
        case Type::ARRAY:   m_val = new internal_class::JsonArray(val.m_val->getArray()); break;
        case Type::BOOLEAN: m_val = val.m_val; break;
        case Type::NUll:    m_val = val.m_val; break;
    }
    return *this;
}
inline Json& Json::operator=(Json&& val){
    m_val = val.m_val;
    val.m_val = nullptr;
    return *this;
}
inline Json& Json::operator=(std::nullptr_t){
    if(type() != Type::NUll){
        if(type() != Type::BOOLEAN){
            delete m_val;
        }
        m_val = &internal_class::Global::json_null();
    }
    return *this;
}
inline Json& Json::operator=(double val){
    if(type() == Type::NUMBER){
        static_cast<internal_class::JsonNumber&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonNumber(val);
    }
    return *this;
}
inline Json& Json::operator=(int val){
    if(type() == Type::NUMBER){
        static_cast<internal_class::JsonNumber&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonNumber(val);
    }
    return *this;
}
inline Json& Json::operator=(bool val){
    if(type() == Type::BOOLEAN){
        m_val = val ? &internal_class::Global::json_true() : &internal_class::Global::json_false();
    }else{
        if(type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonBoolean(val);
    }
    return *this;
}
inline Json& Json::operator=(const std::string& val){
    if(type() == Type::STRING){
        static_cast<internal_class::JsonString&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonString(val);
    }
    return *this;
}
inline Json& Json::operator=(std::string&& val){
    if(type() == Type::STRING){
        static_cast<internal_class::JsonString&>(*m_val) = std::move(val);
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonString(std::move(val));
    }
    return *this;
}
inline Json& Json::operator=(const char* val){
    if(type() == Type::STRING){
        static_cast<internal_class::JsonString&>(*m_val) = std::string(val);
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonString(std::string(val));
    }
    return *this;
}
inline Json& Json::operator=(const Array& val){
    if(type() == Type::ARRAY){
        static_cast<internal_class::JsonArray&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonArray(val);
    }
    return *this;
}
inline Json& Json::operator=(Array&& val){
    if(type() == Type::ARRAY){
        static_cast<internal_class::JsonArray&>(*m_val) = std::move(val);
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonArray(std::move(val));
    }
    return *this;
}
inline Json& Json::operator=(const Object& val){
    if(type() == Type::OBJECT){
        static_cast<internal_class::JsonObject&>(*m_val) = val;
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonObject(val);
    }
    return *this;
}
inline Json& Json::operator=(Object&& val){
    if(type() == Type::OBJECT){
        static_cast<internal_class::JsonObject&>(*m_val) = std::move(val);
    }else{
        if(type() != Type::BOOLEAN && type() != Type::NUll){
            delete m_val;
        }
        m_val = new internal_class::JsonObject(std::move(val));
    }
    return *this;
}

inline void Json::hex(bool val){
    if(type() == Type::NUMBER){
        static_cast<internal_class::JsonNumber&>(*m_val).hex = val;
    }
}
inline void Json::reset(){
    if(type() != Type::BOOLEAN && type() != Type::NUll){
        delete m_val;
    }
    m_val = &internal_class::Global::json_null();
}
inline Type Json::type() const { return m_val->type(); }

inline std::string Json::parse() const {
    std::string temp;
    m_val->parse(temp);
    return temp;
}

namespace json_parser{

class JsonParser{
public:
    size_t index = 0;
    unsigned int line = 1;
    bool fail = false;
    std::string& str;
    std::string error_info = "";
private:
    void error(std::string info){
        fail = true;
        error_info += info + " in line: " + std::to_string(line) + '\n';
    }
public:
    JsonParser(std::string& str) : str(str) {}
    ~JsonParser(){
        if(detail && !error_info.empty()){
            #ifdef _WIN32
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x04);
            #endif
            #ifdef linux
            std::cout << RED;
            #endif

            std::cout << error_info;

            #ifdef _WIN32
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07);
            #endif
            #ifdef linux
            std::cout << NONE;
            #endif
        }
    }
    void skipAnnotation(){
        if(index >= str.size()) return;
        if(str[index] == '/'){
            index++;
            if(index < str.size()){
                if(str[index] == '/'){
                    index++;
                    while(index < str.size() && str[index++] != '\n');
                    if(str[index-1] == '\n') line++;
                }else if(str[index] == '*'){
                    index++;
                    while(!(str[index++] == '*' && str[index++] == '/')){
                        if(index + 1 >= str.size()){
                            error("[TinyJSON] expect '*/' to match '/*'");
                            return;
                        }
                    }
                }else{
                    error("[TinyJSON] expect '/' or '*' after '/'");
                    return;
                }
            }else{
                error("[TinyJSON] only one '/' found");
                return;
            }
        }
    }
    void skipBlanks(){
        while(index < str.size() && (str[index] == '\n'
            || str[index] == '\t' || str[index] == ' ')){
            if(str[index] == '\n') line++;
            ++index;
        }
    }
    const char& moveNext(){
        skipBlanks();
        skipAnnotation();
        skipBlanks();
        if(index >= str.size()){
            index = str.size() - 1;
            return str.back();
        }
        return str[index++];
    }
    Json parseString(){
        std::string content;
        bool mode = str[index++] == '"';
        if(!mode && !JSON5){
            error("[TinyJSON] 'string' only supported in Json5");
            return Json();
        }
        for(; index < str.size(); index++){
            if(str[index] == '\n'){
                // JSON5 string support
                if(!JSON5){
                    error("[TinyJSON] cross line string only supported in Json5");
                    return Json();
                }
                while(index < str.size() && (str[index] == '\n' || str[index] == '\t' || str[index] == ' ')){
                    if(str[index] == '\n') line++;
                    index++;
                }
                index--;
            }else if(str[index] == '\\'){
                index++;
                if(index == str.size()){
                    error("[TinyJSON] expect character after \\");
                    return Json();
                }
                switch(str[index]){
                    case 'b':  content += '\b'; break;
                    case 'f':  content += '\f'; break;
                    case 't':  content += '\t'; break;
                    case 'n':  content += '\n'; break;
                    case 'r':  content += '\r'; break;
                    case '\\': content += '\\'; break;
                    case '"':
                        if(!mode){
                            error("[TinyJSON] \" doesn't need escape in ' '");
                            return Json();
                        }
                        content += '"';
                        break;
                    case '\'':
                        if(mode){
                            error("[TinyJSON] ' doesn't need escape in \" \"");
                            return Json();
                        }
                        content += '\'';
                        break;
                    case 'u':{
                        index++;
                        if(str.size() - 1 - index < 4){
                            error("[TinyJSON] expect 4 characters after \\u");
                            return Json();
                        }
                        // unicode to string
                        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                        char32_t uhan = strtol(str.substr(index, 4).c_str(), nullptr, 16);
                        content += converter.to_bytes(uhan);
                        index += 3;
                        break;
                    }
                    default:
                        error(std::string("[TinyJSON] escape character error: \\") + str[index]);
                        return Json();
                }
            }else if((mode && str[index] == '"') || (!mode && str[index] == '\'')){
                break;
            }else{
                content += str[index];
            }
            if(index == str.size() - 1 && !((mode && str[index] == '"') || (!mode && str[index] == '\''))){
                error("[TinyJSON] expect string wraper");
                return Json();
            }
        }
        index++;
        return Json(content);
    }
    Json parseNumber(){
        bool hex = index + 1 < str.size() && str[index] == '0' && (str[index+1] == 'x');
        std::string temp;
        while(index < str.size() && temp.back() != ',' && temp.back() != ']' && temp.back() != '}'){
            temp += moveNext();
        }
        if(temp.back() == ',' || temp.back() == ']' || temp.back() == '}'){
            temp.pop_back();
            index--;
        }

        Json number;
        if(hex){
            if(!JSON5){
                error("[TinyJSON] hexadecimal numeral only supported in Json5");
                return Json();
            }
            try{
                number = std::stoi(temp, nullptr, 16);
                number.hex(true);
            }catch(std::invalid_argument&){
                error(std::string("[TinyJSON] hexadecimal numeral conversion failed: ") + temp);
                return Json();
            }
        }else{
            try{
                double t = std::stod(temp);
                number = t;
            }catch(std::invalid_argument&){
                error(std::string("[TinyJSON] numeral conversion failed: ") + temp);
                return Json();
            }
        }
        return number;
    }
    Json parseTrue(){
        if(str.size() - index < 4){
            error("[TinyJSON] expect 4 charactors to match true");
        }
        std::string temp = str.substr(index, 4);
        if(temp != "true"){
            error(std::string("[TinyJSON] expect \"true\" but get \"") + temp + '"');
            return Json();
        }
        index += 4;
        return Json(true);
    }
    Json parseFalse(){
        if(str.size() - index < 5){
            error("[TinyJSON] expect 5 charactors to match false");
        }
        std::string temp = str.substr(index, 5);
        if(temp != "false"){
            error(std::string("[TinyJSON] expect \"false\" but get \"") + temp + '"');
            return Json();
        }
        index += 5;
        return Json(false);
    }
    Json parseNull(){
        if(str.size() - index < 4){
            error("[TinyJSON] expect 4 charactors to match null");
        }
        std::string temp = str.substr(index, 4);
        if(temp != "null"){
            error(std::string("[TinyJSON] expect \"null\" but get \"") + temp + '"');
            return Json();
        }
        index += 4;
        return Json();
    }
    Json parseArray(){
        Json arr = Array();
        int count = 0;
        index++;

        while(index < str.size()){
            if(fail) return Json();
            char ch = moveNext();
            if(ch == ']')
                break;
            --index;
            switch(ch){
                case '0'...'9':
                case '-':  arr[count++] = parseNumber(); break;
                case '\'':
                case '"':  arr[count++] = parseString(); break;
                case 't':  arr[count++] = parseTrue();   break;
                case 'f':  arr[count++] = parseFalse();  break;
                case 'n':  arr[count++] = parseNull();   break;
                case '{':  arr[count++] = parseObject(); break;
                case '[':  arr[count++] = parseArray();  break;
                default:
                    error(std::string("[TinyJSON] unkonwn symbol: ") + ch);
                    return Json();
            }
            if(fail) return Json();
            ch = moveNext();
            if(ch == ']'){
                break;
            }else if(ch != ','){
                error(std::string("[TinyJSON] expect ',' but get '") + ch + '\'');
                return Json();
            }
        }
        return arr;
    }
    Json parseObject(){
        if(fail) return Json();
        Json obj = Object();
        std::string key;
        if(moveNext() != '{'){
            error(std::string("[TinyJSON] expect '{' but get '") + str[index] + '\'');
            return Json();
        }

        while(index < str.size()){
            if(fail) return Json();
            char ch = moveNext();
            if(ch == '}')
                break;
            --index;
            if(ch == '"'){
                index++;
                while(index < str.size() && index > 0 && !(str[index-1] != '\\' && str[index] == '"')){
                    key += str[index++];
                }
                index++;
            }else if(JSON5){
                if(ch == '}'){
                    index++;
                    break;
                }
                while(index < str.size() && str[index] != ' ' && str[index] != '\t'
                    && str[index] != '\n' && str[index] != ':' && str[index] != '\r'){
                    if(str[index] == '\n') line++;
                    key += str[index++];
                }
            }else{
                error(std::string("[TinyJSON] expect '\"' but get '") + ch + '\'');
                return Json();
            }
            if(fail) return Json();
            ch = moveNext();
            if(ch != ':'){
                error(std::string("[TinyJSON] expect ':' but get '") + ch + '\'');
                return Json();
            }
            if(fail) return Json();
            ch = moveNext();
            index--;
            switch(ch){
                case '0'...'9':
                case '-': obj[key] = parseNumber(); break;
                case '\'':
                case '"': obj[key] = parseString(); break;
                case 't': obj[key] = parseTrue();   break;
                case 'f': obj[key] = parseFalse();  break;
                case 'n': obj[key] = parseNull();   break;
                case '{': obj[key] = parseObject(); break;
                case '[': obj[key] = parseArray();  break;
                default:
                    error(std::string("[TinyJSON] unkonwn symbol: ") + ch);
                    return Json();
            }
            if(fail) return Json();
            ch = moveNext();
            if(ch == '}'){
                break;
            }else if(ch == ','){
                key.clear();
            }else{
                error(std::string("[TinyJSON] expect ',' but get '") + ch + '\'');
            }
        }
        return obj;
    }
    Json parse(){
        return parseObject();
    }
};

} // json_parser

inline Json parse(std::string& str){
    return json_parser::JsonParser(str).parse();
}
inline Json parse(std::string& str, std::string& error_info){
    auto jp = json_parser::JsonParser(str);
    auto json = jp.parse();
    error_info = jp.error_info;
    return json;
}
inline std::string format(const std::string& json){
    std::string str;
    size_t level = 0;
    auto printBlank = [&]{
        str += '\n';
        for(size_t j = 0; j < level; j++){
            str += '\t';
        }
    };

    bool in_double = false, in_single = false;
    for(size_t i = 0; i < json.size(); i++){
        str += json[i];
        if(!in_double && !in_single){
            if(json[i] == '"'){
                in_double = true;
            }else if(json[i] == '\''){
                in_single = true;
            }
            switch(json[i]){
                case '{':
                    level++;
                    printBlank();
                    break;
                case ',':
                    printBlank();
                    break;
                case '[':
                    level++;
                    printBlank();
                    break;
            }
        }else if(in_double && json[i] == '"' && json[i-1] != '\\'){
            in_double = false;
        }else if(in_single && json[i] == '\'' && json[i-1] != '\\'){
            in_single = false;
        }
        if(!in_double && !in_single){
            if(i + 1 < json.size() && (json[i+1] == '}' || json[i+1] == ']')){
                level--;
                printBlank();
            }
        }
    }
    return str;
}

} // tiny_json
