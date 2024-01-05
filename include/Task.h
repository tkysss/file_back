#ifndef TASK_H
#define TASK_H

//任务管理基本类

#include "Packer.h"
#include "Compressor.h"
#include "Aes.h"

class Task
{
private:
    std::filesystem::path src_path; // 待备份的目录
    std::filesystem::path bak_path; // 备份文件路径(存放位置)

    BackupInfo info; // 备份信息
    Filter filter;   // 用户自定义规则 过滤文件

    bool restore_metadata;  // 是否恢复文件元信息
    bool verbose;           // 是否输出执行过程信息

public:
    Task(std::string src_path_, std::string bak_path_);
    ~Task();

    void setVerbose(bool _verbose);

    // 备份
    bool backup(std::string _password);
    void setComment(std::string _comment);
    void setMod(unsigned char _mod);
    void setFilter(const Filter &_filter);

    // 恢复
    bool restore(std::string password);
    void restoreMetadata(bool _restore_metadata);
    bool getBackupInfo();
    unsigned char getBackupMode();

    // 获取备份文件的基本信息
    static bool getBackupInfo(std::string _file_path, BackupInfo &_info);

};

#endif