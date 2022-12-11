include_directories(${PROJECT_SOURCE_DIR})

include_directories(ThirdParty)

# 禁用 doctest 多线程
add_definitions(-DDOCTEST_CONFIG_NO_MULTITHREADING)

# 定义 performance_test.cpp 的测试文件路径
add_definitions(-DCANADA_JSON="${PROJECT_SOURCE_DIR}/Tests/Json/canada.json")
add_definitions(-DTWITTER_JSON="${PROJECT_SOURCE_DIR}/Tests/Json/twitter.json")
add_definitions(-DCITM_JSON="${PROJECT_SOURCE_DIR}/Tests/Json/citm_catalog.json")

add_executable(Class_Test class_test.cpp)
add_executable(Unit_Test unit_test.cpp)
add_executable(Performance_Test performance_test.cpp)

add_test(NAME class_test COMMAND Class_Test)
add_test(NAME unit_test COMMAND Unit_Test)