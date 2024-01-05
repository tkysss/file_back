#ifndef FILTER_H
#define FILTER_H

//文件过滤器

#include "Global.h"
#include "FileBase.h"
#include<string>

#include<regex>

class Filter
{
private:
    unsigned char type;
    std::string reg_path; //文件路径正则表达式
    std::string reg_name; //文件名正则表达式
    FileType file_type;    //文件类型

    // 限定文件的时间
    time_t atime_start, atime_end;
    time_t mtime_start, mtime_end;
    time_t ctime_start, ctime_end;
public:
    Filter();
    Filter(std::string _fileDir, std::string _fileName, FileType _fileType);
    ~Filter();

    void setPathFilter(std::string reg_path_);
    void setNameFilter(std::string reg_name_);
    void setFileType(FileType file_type_);

    bool check(const FileHeader &fileHeader);
};


#endif