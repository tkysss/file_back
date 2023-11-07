#include<cstdio>
#include<iostream>

#include "task.h"

#include<openssl/aes.h>

using namespace std;

#include "FileBase.h"

#include<ctime>
#include<chrono>
#include<iostream>


int main()
{
    //测试c++编译版本
    // if (__cplusplus == 201703L) std::cout << "C++17\n";
    // else if (__cplusplus == 201402L) std::cout << "C++14\n";
    // else if (__cplusplus == 201103L) std::cout << "C++11\n";
    // else if (__cplusplus == 199711L) std::cout << "C++98\n";
    // else std::cout << "pre-standard C++\n";
 
    //std::cin.get();  //保留命令窗口
 
    auto now = std::chrono::system_clock::now();

    // 将时间点转换为 time_t
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // 使用本地时间对 time_t 进行格式化输出
    std::cout << "Time: " << std::ctime(&now_c) << std::endl;

    return 0;
}