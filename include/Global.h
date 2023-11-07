#ifndef GLOBAL_H
#define GLOBAL_H

//用于宏定义

#define MAX_PATH_LEN 256     //最大文件长度
#define MAX_BUFFER_SIZE 4096 //最大文件的缓冲区大小
#define MAX_COMMENT_LEN      //最大备份长度

//文件系统类型定义，适应stat 结构体
#define FILE_TYPE_NORMAL 1
#define FILE_TYPE_HARD_LINK 2
#define FILE_TYPE_SYMBOLIC_LINK 4
#define FILE_TYPE_DIRECTORY 8
#define FILE_TYPE_FIFO  
#define FILE_TYPE_NORMAL 1

//文件格式
#define COMPRESS_MODE 1 //压缩
#define ENCRYPT_MODE 2  //加密

typedef unsigned char FileType;

#endif