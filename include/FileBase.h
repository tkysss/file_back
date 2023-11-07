#ifndef FILEBASE_H
#define FILEBASE_H



#include "Global.h"

#include<cstring>
#include<iostream>

//_stat 函数头文件 获取文件信息
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>  

//执行文件操作
#include<filesystem>
#include<fstream>

typedef std::filesystem::path FilePath;
#define  fs std::filesystem


// 文件头结构体
struct FileHeader
{
    char name[MAX_PATH_LEN]; //文件存放路径 ??
    struct _stat fileStat;  //存放文件信息

    char linkName[MAX_PATH_LEN]; //链接文件路径
};

//文件系统基本类
class FileBase : public std::fstream
{
    private:
    FileHeader fileHeader; //文件元数据
    FilePath filePath; //文件路径

public:
//     //传入文件元数据构造？
    FileBase(FileHeader _fileHeader);
//     //传入文件路径构造？
    FileBase(FilePath _filePath);
    ~FileBase();

    bool IsHardLink(); //判断是否是硬链接

//     //传入文件路径构造


};








#endif   //FILE_BASE_H