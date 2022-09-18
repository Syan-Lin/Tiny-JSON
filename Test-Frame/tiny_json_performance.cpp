#include "tiny_json_performance.h"

/**************************
* @author   Yuan.
* @date     2022/9/18
* @brief    Performance 类实现
***************************/

void tiny_json_performance::Performance::setScale(int num, tiny_json::Type type){
    if(num > 9999999){
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
    if(num > 9999999){
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
void tiny_json_performance::Performance::run(){
    using namespace tiny_json;
    using namespace std;
    Object obj;
    Log::print("[===============================================================]");
    Log::print("[--------------- Run tiny_json performance test ----------------]");
    Log::print("|     Null  |  Array  |  Object |  String | Number  | Boolean   |");
    Log::print("[----------------------- scale of types ------------------------]");
    print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
    Log::print("[--------------------- construct time (ms) ---------------------]");
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

    Log::print("[----------------------- parse time (ms) -----------------------]");
    string temp = parse(obj);
    auto parse_time = getTime();

    string time = to_string(parse_time);
    string blank((63 - time.size()) / 2, ' ');
    if(time.size() % 2 == 0){
        time = "|" + blank + time + blank + " |";
    }else{
        time = "|" + blank + time + blank + "|";
    }
    Log::print(time);
    Log::print("[===============================================================]");
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
