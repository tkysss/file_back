#ifndef GLOBAL_H
#define GLOBAL_H


#include<cstdlib>
#include<cstring>
#include<iostream>

//用于宏定义

#define MAX_PATH_LEN 256        //最大文件长度
#define MAX_BUFFER_SIZE 4096    //最大文件的缓冲区大小
#define MAX_COMMENT_LEN  4096    //最大备份长度

//文件系统类型定义，适应stat 结构体
#define FILE_TYPE_REG 1 //普通文件
#define FILE_TYPE_HARD_LINK 2 //硬链接
#define FILE_TYPE_SYMBOLIC_LINK 4 //软链接
#define FILE_TYPE_DIR 8 //目录文件
#define FILE_TYPE_FIFO  16 //管道文件
#define FILE_TYPE_OTHER 32 //其他文件

//文件格式
#define MOD_COMPRESS 1 //压缩
#define MOD_ENCRYPT 2  //加密

typedef unsigned int FileType;

#define FILE_SUFFIX_PACK ".pak"
#define FILE_SUFFIX_COMPRESS ".cps"
#define FILE_SUFFIX_ENCRYPT ".ept"

#define ull unsigned long long

// 根据路径/类型/名字/时间筛选备份的文件
#define FILTER_FILE_PATH 1
#define FILTER_FILE_NAME 2
#define FILTER_FILE_TYPE 4
#define FILTER_FILE_ACCESS_TIME 8
#define FILTER_FILE_MODIFY_TIME 16
#define FILTER_FILE_CHANGE_TIME 32

#endif