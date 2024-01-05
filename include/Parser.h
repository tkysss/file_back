#ifndef PARSER_H
#define PARSER_H

//解析命令

#include "Global.h"

#include "cxxopts.hpp"
#include "FileBase.h"

#define ARG_HELP_WIDTH 160

class Parser
{
//调用cxxopts实现简易命令行解析
private:
    cxxopts::Options options; 
    cxxopts::ParseResult result;

    std::string str_type;
    std::string str_atime;
    std::string str_mtime;
    std::string str_ctime;

public:
    // bool变量标识用户是否输入了该参数
    bool flag_backup;
    bool flag_restore;
    bool flag_verbose;
    bool flag_compress;
    bool flag_encrypt;
    bool flag_metadata;
    bool flag_help;

    bool flag_list;
    bool flag_input;
    bool flag_output;
    bool flag_password;
    bool flag_path;
    bool flag_type;
    bool flag_name;
    bool flag_atime;
    bool flag_mtime;
    bool flag_ctime;
    bool flag_message;

    // 具体参数的值
    std::string str_list;
    std::string str_input;
    std::string str_output;
    std::string str_password;
    std::string str_path;
    std::string str_name;
    std::string str_message;
    FileType file_type;
    time_t atime_start, atime_end;
    time_t mtime_start, mtime_end;
    time_t ctime_start, ctime_end;

public:
    Parser();
    ~Parser();

    // 解析命令行参数
    bool parse(int argc, char **argv);

    // 输出错误提示信息
    void errPrint();

    bool checkPassword();

private:
    // 参数格式检查
    bool checkRegExp();
};


#endif