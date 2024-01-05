#ifndef PACKER_H
#define PACKER_H

//打包功能基本类

#include "Global.h"
#include "FileBase.h"
#include "Filter.h"


class Packer {
private: 
    FilePath root_path; //备份文件路径或者恢复文件路径
    FilePath bak_path;  // 打包文件的路径
    Filter filter; //文件过滤器
    //这里本来还有一个对硬链接文件的处理，保证在备份的文件中不会存在重复的硬链接，减少冗余

    bool verbose; // 输出执行过程信息
public:
    Packer(std::string _root_path, std::string _pack_path, const Filter &_filter, bool _verbose = false);
    ~Packer();

    void fileDfs(FileBase &bak_file, FilePath cur_path);
    bool pack(); //打包
    bool unpack(bool restore_fileState); //解包
};


#endif