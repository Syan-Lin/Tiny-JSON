#include "tiny_json_performance.h"

/**************************
* @author   Yuan.
* @date     2022/9/18
* @brief    Performance 类实现
***************************/

#define MAX_LENGTH 9999999

void tiny_json_performance::Performance::setScale(int num, tiny_json::Type type){
    if(num > MAX_LENGTH){
        Log::error("print", "Scale too big!");
        return;
    }
    switch(type){
        case tiny_json::Type::kArray:
            array_num_ = num;
            break;
        case tiny_json::Type::kBoolean:
            bool_num_ = num;
            break;
        case tiny_json::Type::kString:
            string_num_ = num;
            break;
        case tiny_json::Type::kNumber:
            number_num_ = num;
            break;
        case tiny_json::Type::kObject:
            object_num_ = num;
            break;
        case tiny_json::Type::kNull:
            null_num_ = num;
            break;
    }
}
void tiny_json_performance::Performance::setScale(int num){
    if(num > MAX_LENGTH){
        Log::error("print", "Scale too big!");
        return;
    }
    array_num_ = num;
    bool_num_ = num;
    string_num_ = num;
    number_num_ = num;
    object_num_ = num;
    null_num_ = num;
}
DWORD tiny_json_performance::Performance::getTime(){
    time_start_ = time_end_;
    time_end_ = GetTickCount();
    return time_end_ - time_start_;
}
void tiny_json_performance::Performance::readBenchmark(){
    using namespace tiny_json;
    using namespace std;
    Object obj;
    int sum = null_num_ + array_num_ + object_num_ + string_num_ + number_num_ + bool_num_;
    obj.rehash(sum * 2);   // 设置槽数确保其不会影响性能测试
    Log::print("[===============================================================]");
    Log::print("[--------------- Run tiny_json performance test ----------------]");
    Log::print("[----------------------- Read benchmark ------------------------]");
    Log::print("|     Null  |  Array  |  Object |  String | Number  | Boolean   |");
    Log::print("[---------------- Scale of types in one object -----------------]");
    print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
    // 构造 Null
    for(int i = 0; i < null_num_; i++){
        string tag = "#null" + to_string(i);
        obj[tag] = Null();
    }
    // 构造 Array
    for(int i = 0; i < array_num_; i++){
        string tag = "#array" + to_string(i);
        obj[tag] = Value(Array({1, "string", true, Null()}));
    }
    // 构造 Object
    for(int i = 0; i < object_num_; i++){
        string tag = "#object" + to_string(i);
        obj[tag] = Value(Object({{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", Null()}}));
    }
    // 构造 String
    for(int i = 0; i < string_num_; i++){
        string tag = "#string" + to_string(i);
        obj[tag] = Value("Hello World!");
    }
    // 构造 Number
    for(int i = 0; i < number_num_; i++){
        string tag = "#number" + to_string(i);
        obj[tag] = Value(31415);
    }
    // 构造 Boolean
    for(int i = 0; i < bool_num_; i++){
        string tag = "#bool" + to_string(i);
        obj[tag] = Value(true);
    }
    string temp = parse(obj);
    getTime();
    Object init = parse(temp);
    Log::print("[------------------------ Read JSON (ms) -----------------------]");
    printLine("str_size: " + to_string(temp.size()) + ", " + to_string(getTime()) + "ms");
    Log::print("[===============================================================]\n");
}
void tiny_json_performance::Performance::writeBenchmark(){
    using namespace tiny_json;
    using namespace std;
    Object obj;
    int sum = null_num_ + array_num_ + object_num_ + string_num_ + number_num_ + bool_num_;
    obj.rehash(sum * 2);   // 设置槽数确保其不会影响性能测试
    Log::print("[===============================================================]");
    Log::print("[--------------- Run tiny_json performance test ----------------]");
    Log::print("[----------------------- Write benchmark -----------------------]");
    Log::print("|     Null  |  Array  |  Object |  String | Number  | Boolean   |");
    Log::print("[---------------- Scale of types in one object -----------------]");
    print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
    // 构造 Null
    for(int i = 0; i < null_num_; i++){
        string tag = "#null" + to_string(i);
        obj[tag] = Null();
    }
    // 构造 Array
    for(int i = 0; i < array_num_; i++){
        string tag = "#array" + to_string(i);
        obj[tag] = Value(Array({1, "string", true, Null()}));
    }
    // 构造 Object
    for(int i = 0; i < object_num_; i++){
        string tag = "#object" + to_string(i);
        obj[tag] = Value(Object({{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", Null()}}));
    }
    // 构造 String
    for(int i = 0; i < string_num_; i++){
        string tag = "#string" + to_string(i);
        obj[tag] = Value("Hello World!");
    }
    // 构造 Number
    for(int i = 0; i < number_num_; i++){
        string tag = "#number" + to_string(i);
        obj[tag] = Value(31415);
    }
    // 构造 Boolean
    for(int i = 0; i < bool_num_; i++){
        string tag = "#bool" + to_string(i);
        obj[tag] = Value(true);
    }
    Log::print("[----------------------- Write JSON (ms) -----------------------]");
    getTime();
    string temp = parse(obj);
    printLine(to_string(getTime()));
    Log::print("[===============================================================]\n");
}
void tiny_json_performance::Performance::classBenchmark(){
    using namespace tiny_json;
    using namespace std;
    Object obj;
    int sum = null_num_ + array_num_ + object_num_ + string_num_ + number_num_ + bool_num_;
    obj.rehash(sum * 2);   // 设置槽数确保其不会影响性能测试
    Log::print("[===============================================================]");
    Log::print("[--------------- Run tiny_json performance test ----------------]");
    Log::print("[---------------------- Class benchmark ------------------------]");
    Log::print("|     Null  |  Array  |  Object |  String | Number  | Boolean   |");
    Log::print("[---------------- Scale of types in one object -----------------]");
    print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
    Log::print("[--------------------- Construct time (ms) ---------------------]");
    array<int, 6> cst_time;
    getTime();
    // 构造 Null
    for(int i = 0; i < null_num_; i++){
        string tag = "#null" + to_string(i);
        obj[tag] = Null();
    }
    cst_time[0] = getTime();
    // 构造 Array
    for(int i = 0; i < array_num_; i++){
        string tag = "#array" + to_string(i);
        obj[tag] = Value(Array({1, "string", true, Null()}));
    }
    cst_time[1] = getTime();
    // 构造 Object
    for(int i = 0; i < object_num_; i++){
        string tag = "#object" + to_string(i);
        obj[tag] = Value(Object({{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", Null()}}));
    }
    cst_time[2] = getTime();
    // 构造 String
    for(int i = 0; i < string_num_; i++){
        string tag = "#string" + to_string(i);
        obj[tag] = Value("Hello World!");
    }
    cst_time[3] = getTime();
    // 构造 Number
    for(int i = 0; i < number_num_; i++){
        string tag = "#number" + to_string(i);
        obj[tag] = Value(31415);
    }
    cst_time[4] = getTime();
    // 构造 Boolean
    for(int i = 0; i < bool_num_; i++){
        string tag = "#bool" + to_string(i);
        obj[tag] = Value(true);
    }
    cst_time[5] = getTime();
    print(cst_time);

    array<int, 6> dst_time;
    // 析构 Null
    for(int i = 0; i < null_num_; i++){
        string tag = "#null" + to_string(i);
        obj.remove(tag);
    }
    dst_time[0] = getTime();
    // 析构 Array
    for(int i = 0; i < array_num_; i++){
        string tag = "#array" + to_string(i);
        obj.remove(tag);
    }
    dst_time[1] = getTime();
    // 析构 Object
    for(int i = 0; i < object_num_; i++){
        string tag = "#object" + to_string(i);
        obj.remove(tag);
    }
    dst_time[2] = getTime();
    // 析构 String
    for(int i = 0; i < string_num_; i++){
        string tag = "#string" + to_string(i);
        obj.remove(tag);
    }
    dst_time[3] = getTime();
    // 析构 Number
    for(int i = 0; i < number_num_; i++){
        string tag = "#number" + to_string(i);
        obj.remove(tag);
    }
    dst_time[4] = getTime();
    // 析构 Boolean
    for(int i = 0; i < bool_num_; i++){
        string tag = "#bool" + to_string(i);
        obj.remove(tag);
    }
    dst_time[5] = getTime();
    Log::print("[--------------------- Destruct time (ms) ----------------------]");
    print(dst_time);
    Log::print("[===============================================================]\n");
}
void tiny_json_performance::Performance::runLoop(int loop){
    using namespace tiny_json;
    using namespace std;
    Log::print("[===============================================================]");
    Log::print("[--------------- Run tiny_json performance test ----------------]");
    Log::print("|     Null  |  Array  |  Object |  String | Number  | Boolean   |");
    Log::print("[---------------- Scale of types in one object -----------------]");
    print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
    Log::print("[---------------------------- Loops ----------------------------]");
    printLine(to_string(loop));
    getTime();
    string json;
    for(int i = 0; i < loop; i++){
        Object obj;
        // 构造 Null
        for(int i = 0; i < null_num_; i++){
            string tag = "#null" + to_string(i);
            obj[tag] = Null();
        }
        // 构造 Array
        for(int i = 0; i < array_num_; i++){
            string tag = "#array" + to_string(i);
            obj[tag] = Value(Array({1, "string", true, Null()}));
        }
        // 构造 Object
        for(int i = 0; i < object_num_; i++){
            string tag = "#object" + to_string(i);
            obj[tag] = Value(Object({{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", Null()}}));
        }
        // 构造 String
        for(int i = 0; i < string_num_; i++){
            string tag = "#string" + to_string(i);
            obj[tag] = Value("Hello World!");
        }
        // 构造 Number
        for(int i = 0; i < number_num_; i++){
            string tag = "#number" + to_string(i);
            obj[tag] = Value(31415);
        }
        // 构造 Boolean
        for(int i = 0; i < bool_num_; i++){
            string tag = "#bool" + to_string(i);
            obj[tag] = Value(true);
        }
        json = parse(obj);
    }
    Log::print("[--------------------- Construct time (ms) ---------------------]");
    Log::print("[--------------------- Includes write JSON ---------------------]");
    printLine(to_string(getTime()));
    Log::print("[===============================================================]\n");
}
void tiny_json_performance::Performance::printLine(std::string str){
    if(str.size() > 63) return;
    std::string blank((63 - str.size()) / 2, ' ');
    if(str.size() % 2 == 0){
        str = "|" + blank + str + blank + " |";
    }else{
        str = "|" + blank + str + blank + "|";
    }
    Log::print(str);
}
void tiny_json_performance::Performance::print(std::array<int, 6> arr){
    using namespace std;
    Log::setColor(Color::kDefault);
    cout << "|  ";
    for(int i = 0; i < arr.size(); i++){
        string temp = to_string(arr[i]);
        if(temp.size() > 7){
            Log::error("print", "Out of space!");
            return;
        }
        if(i == 5){
            cout << "  " << temp << string(7 - temp.size(), ' ') << "  |" << endl;
        }else{
            cout << "  " << temp << string(7 - temp.size(), ' ') << "|";
        }
    }
}
