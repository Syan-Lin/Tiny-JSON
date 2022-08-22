#include "tiny_json.h"

// 判断对象是否可转化为字符串（可能出现环导致死循环）
static bool parseable(const tiny_json::Object&);
// 判断字符串是否可转化为对象
static bool parseable(std::string){
    return true;
}
