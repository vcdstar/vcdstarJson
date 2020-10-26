#pragma once

#include "vcdstarJson.h"
using namespace vcdstarJson;

// 基本对象方法
void base_test(Json j)
{
	cout << j.toJsonString() << endl;

	Json j1 = j;
	cout << j1.toJsonString() << endl;
	j1 = j;
	cout << j1.toJsonString() << endl;

	Json j2;
	j2 = j1;
	cout << j2.toJsonString() << endl;

	Json j3 = j["test1"];
	cout << j3.toJsonString() << endl;

	Json j4 = j["test7"];
	cout << j4.toJsonString() << endl;

	Json j5 = j["test7"];
	cout << j5.toJsonString() << endl;
}

// test =int
void operator_int(Json j)
{
	// 数组
	j["test7"][0] = 111;
	j["test7"][1] = 111;
	j["test7"][2] = 111;
	j["test7"][3] = 111;
	j["test7"][4] = 111;
	j["test7"][5] = 111;
	j["test7"][6] = 111;
	cout << j.toJsonString() << endl;
	// 对象
	j["test6"]["test1"] = 111;
	j["test6"]["test2"] = 111;
	j["test6"]["test3"] = 111;
	j["test6"]["test4"] = 111;
	j["test6"]["test5"] = 111;
	j["test6"]["test6"] = 111;
	j["test6"]["test7"] = 111;
	cout << j.toJsonString() << endl;
	// 空数组
	j["test9"].add(999);
	j["test9"].add(888);
	j["test9"].add(777);
	cout << j.toJsonString() << endl;
	// 数组insert
	j["test9"].insert(0, 555);
	j["test9"].insert(1, 222);
	j["test9"].insert(2, 333);
	j["test9"].insert(3, 444);
	cout << j.toJsonString() << endl;
	// 空对象
	j["test8"]["test1"] = 999;
	j["test8"]["test1"] = 888;
	j["test8"]["test1"] = 777;
	j["test8"]["test1"] = 666;
	cout << j.toJsonString() << endl;
	// 整个替换=int
	j["test1"] = 111;
	j["test2"] = 111;
	j["test3"] = 111;
	j["test4"] = 111;
	j["test5"] = 111;
	j["test6"] = 111;
	j["test7"] = 111;
	j["test8"] = 111;
	j["test9"] = 111;
	cout << j.toJsonString() << endl;
	// 空json=int
	Json j1;
	j1["test1"] = 111;
	j1["test2"] = 222;
	j1["test3"] = 333;
	j1["test1"] = 444;
	j1.add(111);
	j1.insert(0, 222);
	cout << j1.toJsonString() << endl;
	// 空json add 和 insert int
	Json j2;
	j2.createEmptyArray();
	j1["test1"] = 111;
	j2.add(111);
	j2.add(111);
	j2.insert(0, 222);
	j2.insert(1, 333);
	j2.insert(2, 444);
	cout << j2.toJsonString() << endl;
}
// test =double
void operator_double(Json j)
{
	// 数组
	j["test7"][0] = 0.1234;
	j["test7"][1] = 0.1234;
	j["test7"][2] = 0.1234;
	j["test7"][3] = 0.1234;
	j["test7"][4] = 0.1234;
	j["test7"][5] = 0.1234;
	j["test7"][6] = 0.1234;
	cout << j.toJsonString() << endl;
	// 对象
	j["test6"]["test1"] = 0.1234;
	j["test6"]["test2"] = 0.1234;
	j["test6"]["test3"] = 0.1234;
	j["test6"]["test4"] = 0.1234;
	j["test6"]["test5"] = 0.1234;
	j["test6"]["test6"] = 0.1234;
	j["test6"]["test7"] = 0.1234;
	cout << j.toJsonString() << endl;
	// 空数组
	j["test9"].add(0.999);
	j["test9"].add(0.888);
	j["test9"].add(0.777);
	cout << j.toJsonString() << endl;
	// 数组insert
	j["test9"].insert(0, 0.555);
	j["test9"].insert(1, 0.222);
	j["test9"].insert(2, 0.333);
	j["test9"].insert(3, 0.444);
	cout << j.toJsonString() << endl;
	// 空对象
	j["test8"]["test1"] = 0.999;
	j["test8"]["test1"] = 0.888;
	j["test8"]["test1"] = 0.777;
	j["test8"]["test1"] = 0.666;
	cout << j.toJsonString() << endl;
	// 整个替换=int
	j["test1"] = 0.1234;
	j["test2"] = 0.1234;
	j["test3"] = 0.1234;
	j["test4"] = 0.1234;
	j["test5"] = 0.1234;
	j["test6"] = 0.1234;
	j["test7"] = 0.1234;
	j["test8"] = 0.1234;
	j["test9"] = 0.1234;
	cout << j.toJsonString() << endl;
	// 空json=int
	Json j1;
	j1["test1"] = 0.1234;
	j1["test2"] = 0.222;
	j1["test3"] = 0.333;
	j1["test1"] = 0.444;
	j1.add(0.1234);
	j1.insert(0, 222);
	cout << j1.toJsonString() << endl;
	// 空json add 和 insert int
	Json j2;
	j2.createEmptyArray();
	j1["test1"] = 0.1234;
	j2.add(0.1234);
	j2.add(0.1234);
	j2.insert(0, 0.222);
	j2.insert(1, 0.333);
	j2.insert(2, 0.444);
	cout << j2.toJsonString() << endl;
}
// test =bool
void operator_bool(Json j)
{
	// 数组
	j["test7"][0] = false;
	j["test7"][1] = true;
	j["test7"][2] = false;
	j["test7"][3] = true;
	j["test7"][4] = false;
	j["test7"][5] = true;
	j["test7"][6] = false;
	cout << j.toJsonString() << endl;
	// 对象
	j["test6"]["test1"] = false;
	j["test6"]["test2"] = true;
	j["test6"]["test3"] = false;
	j["test6"]["test4"] = true;
	j["test6"]["test5"] = false;
	j["test6"]["test6"] = true;
	j["test6"]["test7"] = false;
	cout << j.toJsonString() << endl;
	// 空数组
	j["test9"].add(true);
	j["test9"].add(false);
	j["test9"].add(true);
	cout << j.toJsonString() << endl;
	// 数组insert
	j["test9"].insert(0, false);
	j["test9"].insert(1, false);
	j["test9"].insert(2, true);
	j["test9"].insert(3, true);
	cout << j.toJsonString() << endl;
	// 空对象
	j["test8"]["test1"] = true;
	j["test8"]["test1"] = false;
	j["test8"]["test1"] = true;
	j["test8"]["test1"] = false;
	cout << j.toJsonString() << endl;
	// 整个替换=int
	j["test1"] = false;
	j["test2"] = true;
	j["test3"] = false;
	j["test4"] = true;
	j["test5"] = false;
	j["test6"] = true;
	j["test7"] = false;
	j["test8"] = true;
	j["test9"] = false;
	cout << j.toJsonString() << endl;
	// 空json=int
	Json j1;
	j1["test1"] = true;
	j1["test2"] = false;
	j1["test3"] = true;
	j1["test1"] = false;
	j1[0]["test"] = false;
	j1.add(true);
	j1.insert(0, true);
	cout << j1.toJsonString() << endl;
	// 空json add 和 insert int
	Json j2;
	j2.createEmptyArray();
	j2.add(true);
	j2.add(true);
	j2.insert(0, true);
	j2.insert(1, true);
	j2.insert(2, true);
	j2["test1"] = true;
	j2["test2"] = false;
	j2["test3"] = true;
	cout << j2.toJsonString() << endl;
}


// test =string
void operator_string(Json j)
{
	// 数组
	j["test7"][0] = string("111");
	j["test7"][1] = string("111");
	j["test7"][2] = string("111");
	j["test7"][3] = string("111");
	j["test7"][4] = string("111");
	j["test7"][5] = string("111");
	j["test7"][6] = string("111");
	cout << j.toJsonString() << endl;
	// 对象
	j["test6"]["test1"] = string("111");
	j["test6"]["test2"] = string("111");
	j["test6"]["test3"] = string("111");
	j["test6"]["test4"] = string("111");
	j["test6"]["test5"] = string("111");
	j["test6"]["test6"] = string("111");
	j["test6"]["test7"] = string("111");
	cout << j.toJsonString() << endl;
	// 空数组
	j["test9"].add(string("999"));
	j["test9"].add(string("888"));
	j["test9"].add(string("777"));
	cout << j.toJsonString() << endl;
	// 数组insert
	j["test9"].insert(0, string("555"));
	j["test9"].insert(1, string("222"));
	j["test9"].insert(2, string("333"));
	j["test9"].insert(3, string("444"));
	cout << j.toJsonString() << endl;
	// 空对象
	j["test8"]["test1"] = string("999");
	j["test8"]["test1"] = string("888");
	j["test8"]["test2"] = string("777");
	j["test8"]["test3"] = string("666");
	cout << j.toJsonString() << endl;
	// 整个替换=int
	j["test1"] = string("111");
	j["test2"] = string("111");
	j["test3"] = string("111");
	j["test4"] = string("111");
	j["test5"] = string("111");
	j["test6"] = string("111");
	j["test7"] = string("111");
	j["test8"] = string("111");
	j["test9"] = string("111");
	cout << j.toJsonString() << endl;
	// 空json=int
	Json j1;
	j1["test1"] = string("111");
	j1["test2"] = string("222");
	j1["test3"] = string("333");
	j1["test1"] = string("444");
	j1.add(string("111"));
	j1.insert(0, string("222"));
	cout << j1.toJsonString() << endl;
	// 空json add 和 insert int
	Json j2;
	j2.createEmptyArray();
	j1["test1"] = string("111");
	j2.add(string("111"));
	j2.add(string("111"));
	j2.insert(0, string("222"));
	j2.insert(1, string("333"));
	j2.insert(2, string("444"));
	cout << j2.toJsonString() << endl;
}

// test =null
void operator_null(Json j)
{
	// 数组
	j["test7"][0] = nullptr;
	j["test7"][1] = nullptr;
	j["test7"][2] = nullptr;
	j["test7"][3] = nullptr;
	j["test7"][4] = nullptr;
	j["test7"][5] = nullptr;
	j["test7"][6] = nullptr;
	cout << j.toJsonString() << "--size:" << j["test7"].arrLen() << endl;
	// 对象
	j["test6"]["test1"] = nullptr;
	j["test6"]["test2"] = nullptr;
	j["test6"]["test3"] = nullptr;
	j["test6"]["test4"] = nullptr;
	j["test6"]["test5"] = nullptr;
	j["test6"]["test6"] = nullptr;
	j["test6"]["test7"] = nullptr;
	cout << j.toJsonString() << endl;
	// 空数组
	j["test9"].add();
	j["test9"].add();
	j["test9"].add();
	cout << j.toJsonString() << "--size:" << j["test9"].arrLen() << endl;
	// 数组insert
	j["test9"].insert(0);
	j["test9"].insert(1);
	j["test9"].insert(2);
	j["test9"].insert(3);
	cout << j.toJsonString() << "--size:" << j["test9"].arrLen() << endl;
	// 空对象
	j["test8"]["test1"] = nullptr;
	j["test8"]["test1"] = nullptr;
	j["test8"]["test2"] = nullptr;
	j["test8"]["test3"] = nullptr;
	cout << j.toJsonString() << endl;
	// 整个替换=int
	j["test1"] = nullptr;
	j["test2"] = nullptr;
	j["test3"] = nullptr;
	j["test4"] = nullptr;
	j["test5"] = nullptr;
	j["test6"] = nullptr;
	j["test7"] = nullptr;
	j["test8"] = nullptr;
	j["test9"] = nullptr;
	cout << j.toJsonString() << endl;
	// 空json=int
	Json j1;
	j1["test1"] = nullptr;
	j1["test2"] = nullptr;
	j1["test3"] = nullptr;
	j1["test1"] = nullptr;
	j1.add();
	j1.insert(0);
	cout << j1.toJsonString() << "--size:" << j1.arrLen() << endl;
	// 空json add 和 insert int
	Json j2;
	j2.createEmptyArray();
	j1["test1"] = nullptr;
	j2.add();
	j2.add();
	j2.insert(0);
	j2.insert(1);
	j2.insert(2);
	cout << j2.toJsonString() << "--size:" << j2.arrLen() << endl;
}

