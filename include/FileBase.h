#ifndef FILEBASE_H
#define FILEBASE_H



#include "Global.h"


#include<cstdint>
#include<ctime>

//_stat 函数头文件 获取文件信息
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<windows.h>

//执行文件操作
#include<filesystem>
#include<fstream>
// #include <boost/filesystem.hpp>

typedef std::filesystem::path FilePath;
#define fs std::filesystem


// 文件头结构体
// 文件元数据
struct FileHeader
{
    char name[MAX_PATH_LEN]; //文件路径 
    struct _stat fileStat;  //存放文件信息
    
    int linkType;//-1 表示未指定/文件已存在 0 表示不为链接文件 1表示为硬连接 2表示为软链接
    char linkName[MAX_PATH_LEN]; //链接文件路径 
    //如果文件是软链接，在打包时需要检测连接路径，只有链接路径在备份路径里才能有效，并且需要修改对应的路径，解包同理
};

//备份信息
//备份信息会被写入FileBase中
struct BackupInfo
{
    //校验和的实现
    //将数据补成mod 2^32 = 0的形式
    //补出的uint32_t就是校验和
    //注意一定要保证指针能访问到这个uint32_t变量
    time_t timestamp;               // 时间戳
    uint32_t checksum;              // 校验和
    char backup_path[MAX_PATH_LEN]; // 备份路径
    char comment[MAX_COMMENT_LEN];  // 描述信息
    unsigned char mod;              // 压缩、加密

    uint32_t calcCheckSum() //使用32位无符号整数计算校验和
    {
        uint32_t *p = (uint32_t *)this;
        int cnt = sizeof(BackupInfo) / 4;
        uint32_t sum = 0;
        for(int i=0;i<cnt;++i)
        {
            sum += *p;
            p++;
        }
        return sum;
    } 
};


//文件系统基本类
class FileBase : public std::fstream
{
    private:
    FileHeader fileHeader; //文件元数据
    FilePath filePath; //文件路径,用于一些基本路径处理操作，比如找parent directory

public:
    FileBase(FileHeader _fileHeader);//传入文件元数据，构造文件类并创建文件
    FileBase(FilePath _filePath);//传入文件路径，构造文件类但不创建文件
    ~FileBase();

    bool isHardLink(); //判断是否是硬链接
    bool isSymbolicLink();//按断是否是软链接
    FileType getFileType();//返回该结构体文件类型

    FileHeader getFileHeader();//返回文件元数据信息

    static FileType getFileType(const FileHeader &_fileHeader);//根据文件元数据返回文件类型

    bool openFile(std::ios_base::openmode _mode);//fsteam 打开文件
    BackupInfo readBackupInfo(); //返回文件备份信息
    void writeBackupInfo(BackupInfo _backupInfo); //写入文件备份信息
    void writeBackupInfo(); //写入文件备份信息
    FileHeader readFileHeader(); //返回文件头,在备份文件中可以通过文件头获取备份的文件列表
    void writeFileHeader(FileHeader _fileHeader); //写入文件头，在备份文件中写入备份的所有文件的文件头

    void restoreFileStat(); //在解包文件时返回文件的时间
    void copyFile(std::fstream file);

    size_t getFileSize(); //


};








#endif   //FILE_BASE_H