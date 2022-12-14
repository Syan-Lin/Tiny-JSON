#include <iostream>
#include <thread>
#include <array>
#include "tiny_json.h"

using namespace std;
using namespace tiny_json;

#define MAX_LENGTH 9999999

#ifndef CANADA_JSON
#define CANADA_JSON "Json/canada.json"
#endif
#ifndef TWITTER_JSON
#define TWITTER_JSON "Json/twitter.json"
#endif
#ifndef CITM_JSON
#define CITM_JSON "Json/citm_catalog.json"
#endif

class Performance{
public:
    void setScale(int num, tiny_json::Type type){
        if(num > MAX_LENGTH){
            cout << "Scale too big!" << endl;
            return;
        }
        switch(type){
            case tiny_json::Type::ARRAY:
                array_num_ = num;
                break;
            case tiny_json::Type::BOOLEAN:
                bool_num_ = num;
                break;
            case tiny_json::Type::STRING:
                string_num_ = num;
                break;
            case tiny_json::Type::NUMBER:
                number_num_ = num;
                break;
            case tiny_json::Type::OBJECT:
                object_num_ = num;
                break;
            case tiny_json::Type::NUll:
                null_num_ = num;
                break;
        }
    }
    void setScale(int num){
        if(num > MAX_LENGTH){
            cout << "Scale too big!" << endl;
            return;
        }
        array_num_ = num;
        bool_num_ = num;
        string_num_ = num;
        number_num_ = num;
        object_num_ = num;
        null_num_ = num;
    }
    void classBenchmark(){
        Json obj = Object();
        int sum = null_num_ + array_num_ + object_num_ + string_num_ + number_num_ + bool_num_;
        cout << "[===============================================================]" << endl;
        cout << "[--------------- Run tiny_json performance test ----------------]" << endl;
        cout << "[---------------------- Class benchmark ------------------------]" << endl;
        cout << "|     Null  |  Array  |  Object |  String | Number  | Boolean   |" << endl;
        cout << "[---------------- Scale of types in one object -----------------]" << endl;
        print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
        cout << "[--------------------- Construct time (ms) ---------------------]" << endl;
        array<int, 6> cst_time;
        getTime();
        // ?????? Null
        for(int i = 0; i < null_num_; i++){
            string tag = "#null" + to_string(i);
            obj[tag] = nullptr;
        }
        cst_time[0] = getTime();
        // ?????? Array
        for(int i = 0; i < array_num_; i++){
            string tag = "#array" + to_string(i);
            obj[tag] = Array({1, "string", true, nullptr});
        }
        cst_time[1] = getTime();
        // ?????? Object
        for(int i = 0; i < object_num_; i++){
            string tag = "#object" + to_string(i);
            obj[tag] = Object({{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", nullptr}});
        }
        cst_time[2] = getTime();
        // ?????? String
        for(int i = 0; i < string_num_; i++){
            string tag = "#string" + to_string(i);
            obj[tag] = "Hello World!";
        }
        cst_time[3] = getTime();
        // ?????? Number
        for(int i = 0; i < number_num_; i++){
            string tag = "#number" + to_string(i);
            obj[tag] = 31415;
        }
        cst_time[4] = getTime();
        // ?????? Boolean
        for(int i = 0; i < bool_num_; i++){
            string tag = "#bool" + to_string(i);
            obj[tag] = true;
        }
        cst_time[5] = getTime();
        print(cst_time);

        array<int, 6> dst_time;
        auto& mp = obj.getMap();
        // ?????? Null
        for(int i = 0; i < null_num_; i++){
            string tag = "#null" + to_string(i);
            mp.erase(tag);
        }
        dst_time[0] = getTime();
        // ?????? Array
        for(int i = 0; i < array_num_; i++){
            string tag = "#array" + to_string(i);
            mp.erase(tag);
        }
        dst_time[1] = getTime();
        // ?????? Object
        for(int i = 0; i < object_num_; i++){
            string tag = "#object" + to_string(i);
            mp.erase(tag);
        }
        dst_time[2] = getTime();
        // ?????? String
        for(int i = 0; i < string_num_; i++){
            string tag = "#string" + to_string(i);
            mp.erase(tag);
        }
        dst_time[3] = getTime();
        // ?????? Number
        for(int i = 0; i < number_num_; i++){
            string tag = "#number" + to_string(i);
            mp.erase(tag);
        }
        dst_time[4] = getTime();
        // ?????? Boolean
        for(int i = 0; i < bool_num_; i++){
            string tag = "#bool" + to_string(i);
            mp.erase(tag);
        }
        dst_time[5] = getTime();
        cout << "[--------------------- Destruct time (ms) ----------------------]" << endl;
        print(dst_time);
        cout << "[===============================================================]\n" << endl;
    }
    void readBenchmark(){
        Json obj = Object();
        int sum = null_num_ + array_num_ + object_num_ + string_num_ + number_num_ + bool_num_;
        cout << "[===============================================================]" << endl;
        cout << "[--------------- Run tiny_json performance test ----------------]" << endl;
        cout << "[----------------------- Read benchmark ------------------------]" << endl;
        cout << "|     Null  |  Array  |  Object |  String | Number  | Boolean   |" << endl;
        cout << "[---------------- Scale of types in one object -----------------]" << endl;
        print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
        // ?????? Null
        for(int i = 0; i < null_num_; i++){
            string tag = "#null" + to_string(i);
            obj[tag] = nullptr;
        }
        // ?????? Array
        for(int i = 0; i < array_num_; i++){
            string tag = "#array" + to_string(i);
            obj[tag] = Array({1, "string", true, nullptr});
        }
        // ?????? Object
        for(int i = 0; i < object_num_; i++){
            string tag = "#object" + to_string(i);
            obj[tag] = Object({{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", nullptr}});
        }
        // ?????? String
        for(int i = 0; i < string_num_; i++){
            string tag = "#string" + to_string(i);
            obj[tag] = "Hello World!";
        }
        // ?????? Number
        for(int i = 0; i < number_num_; i++){
            string tag = "#number" + to_string(i);
            obj[tag] = 31415;
        }
        // ?????? Boolean
        for(int i = 0; i < bool_num_; i++){
            string tag = "#bool" + to_string(i);
            obj[tag] = true;
        }
        string temp = obj.parse();
        getTime();
        Json init = parse(temp);
        cout << "[------------------------ Read JSON (ms) -----------------------]" << endl;
        printLine("str_size: " + to_string(temp.size()/1000) + " KB, " + to_string(getTime()) + "ms");
        cout << "[===============================================================]\n" << endl;
    }
    void writeBenchmark(){
        Json obj = Object();
        int sum = null_num_ + array_num_ + object_num_ + string_num_ + number_num_ + bool_num_;
        cout << "[===============================================================]" << endl;
        cout << "[--------------- Run tiny_json performance test ----------------]" << endl;
        cout << "[----------------------- Write benchmark -----------------------]" << endl;
        cout << "|     Null  |  Array  |  Object |  String | Number  | Boolean   |" << endl;
        cout << "[---------------- Scale of types in one object -----------------]" << endl;
        print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
        // ?????? Null
        for(int i = 0; i < null_num_; i++){
            string tag = "#null" + to_string(i);
            obj[tag] = nullptr;
        }
        // ?????? Array
        for(int i = 0; i < array_num_; i++){
            string tag = "#array" + to_string(i);
            obj[tag] = Array({1, "string", true, nullptr});
        }
        // ?????? Object
        for(int i = 0; i < object_num_; i++){
            string tag = "#object" + to_string(i);
            obj[tag] = Object({{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", nullptr}});
        }
        // ?????? String
        for(int i = 0; i < string_num_; i++){
            string tag = "#string" + to_string(i);
            obj[tag] = "Hello World!";
        }
        // ?????? Number
        for(int i = 0; i < number_num_; i++){
            string tag = "#number" + to_string(i);
            obj[tag] = 31415;
        }
        // ?????? Boolean
        for(int i = 0; i < bool_num_; i++){
            string tag = "#bool" + to_string(i);
            obj[tag] = true;
        }
        cout << "[----------------------- Write JSON (ms) -----------------------]" << endl;
        getTime();
        string temp = obj.parse();
        printLine("str_size: " + to_string(temp.size()/1000) + " KB, " + to_string(getTime()) + "ms");
        cout << "[===============================================================]\n" << endl;
    }
    void runLoop(int loop){
        cout << "[===============================================================]" << endl;
        cout << "[--------------- Run tiny_json performance test ----------------]" << endl;
        cout << "|     Null  |  Array  |  Object |  String | Number  | Boolean   |" << endl;
        cout << "[---------------- Scale of types in one object -----------------]" << endl;
        print({null_num_, array_num_, object_num_, string_num_, number_num_, bool_num_});
        cout << "[---------------------------- Loops ----------------------------]" << endl;
        printLine(to_string(loop));
        getTime();
        string json;
        for(int i = 0; i < loop; i++){
            Json obj = Object();
            // ?????? Null
            for(int i = 0; i < null_num_; i++){
                string tag = "#null" + to_string(i);
                obj[tag] = nullptr;
            }
            // ?????? Array
            for(int i = 0; i < array_num_; i++){
                string tag = "#array" + to_string(i);
                obj[tag] = Array({1, "string", true, nullptr});
            }
            // ?????? Object
            for(int i = 0; i < object_num_; i++){
                string tag = "#object" + to_string(i);
                obj[tag] = Object({{"number", 1}, {"string", "hello"}, {"bool", false}, {"null", nullptr}});
            }
            // ?????? String
            for(int i = 0; i < string_num_; i++){
                string tag = "#string" + to_string(i);
                obj[tag] = "Hello World!";
            }
            // ?????? Number
            for(int i = 0; i < number_num_; i++){
                string tag = "#number" + to_string(i);
                obj[tag] = 31415;
            }
            // ?????? Boolean
            for(int i = 0; i < bool_num_; i++){
                string tag = "#bool" + to_string(i);
                obj[tag] = true;
            }
            json = obj.parse();
        }
        cout << "[--------------------- Construct time (ms) ---------------------]" << endl;
        cout << "[--------------------- Includes write JSON ---------------------]" << endl;
        printLine("str_size: " + to_string(json.size()/1000) + " KB, " + to_string(getTime()));
        cout << "[===============================================================]\n" << endl;
    }
    time_t getTime(){
        time_point_now = time_point_after;
        time_point_after = chrono::system_clock::now();
        chrono::system_clock::duration duration = time_point_after - time_point_now;
        time_t time = chrono::duration_cast<chrono::milliseconds>(duration).count();
        return time;
    }
private:
    void printLine(std::string str){
        if(str.size() > 63) return;
        std::string blank((63 - str.size()) / 2, ' ');
        if(str.size() % 2 == 0){
            str = "|" + blank + str + blank + " |";
        }else{
            str = "|" + blank + str + blank + "|";
        }
        cout << str << endl;
    }
    void print(std::array<int, 6> arr){
        cout << "|  ";
        for(int i = 0; i < arr.size(); i++){
            string temp = to_string(arr[i]);
            if(temp.size() > 7){
                cout << "Out of space!" << endl;
                return;
            }
            if(i == 5){
                cout << "  " << temp << string(7 - temp.size(), ' ') << "  |" << endl;
            }else{
                cout << "  " << temp << string(7 - temp.size(), ' ') << "|";
            }
        }
    }
    chrono::system_clock::time_point time_point_now, time_point_after;
    int array_num_ = 0;
    int string_num_ = 0;
    int bool_num_ = 0;
    int number_num_ = 0;
    int object_num_ = 0;
    int null_num_ = 0;
};

#include <fstream>
string readFile(string filename){
    ifstream file;
	file.open(filename, ios::in);
	string buff, result;
	while(getline(file, buff)){
		result += buff + '\n';
	}
	file.close();
    cout << "json_size(" + filename + "): " << result.size()/1000 << " KB" << endl;
    return result;
}
template<typename T>
void countTime(T t){
    chrono::system_clock::time_point time_point_now = chrono::system_clock::now();
    t();
    chrono::system_clock::time_point time_point_after = chrono::system_clock::now();
    chrono::system_clock::duration duration = time_point_after - time_point_now;
    time_t time = chrono::duration_cast<chrono::milliseconds>(duration).count();
    cout << "time: " << time << "ms" << endl;
}

int main(){
    Performance pt;
    pt.setScale(1000);
    pt.readBenchmark();
    pt.setScale(10000);
    pt.writeBenchmark();
    pt.readBenchmark();
    pt.setScale(100000);
    pt.classBenchmark();
    pt.setScale(10, Type::NUll);
    pt.setScale(20, Type::NUMBER);
    pt.setScale(50, Type::OBJECT);
    pt.setScale(50, Type::STRING);
    pt.setScale(10, Type::BOOLEAN);
    pt.setScale(20, Type::ARRAY);
    pt.runLoop(1000);
    pt.runLoop(10000);

    string twitter = readFile(TWITTER_JSON);
    countTime([&]{
        parse(twitter);
    });
    string canada = readFile(CANADA_JSON);
    countTime([&]{
        parse(canada);
    });
    string citm = readFile(CITM_JSON);
    countTime([&]{
        parse(citm);
    });

    return 0;
}
