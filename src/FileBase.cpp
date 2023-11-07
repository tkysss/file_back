#include "FileBase.h"

FileBase::FileBase(FileHeader _fileHeader)
{
    this -> fileHeader = _fileHeader;
    filePath.assign(_fileHeader.name);
    
    //硬链接文件
    if(IsHardLink()){
        fs::remove_all(fileHeader.name);//要求硬链接文件不能已经存在
        fs::create_hard_link(fileHeader.linkName, fileHeader.name); //此时fileHeader.linkName是源文件地址，fileHeader.name 是链接文件地址
    }

}


bool FileBase::IsHardLink()
{
    if(fileHeader.fileStat.st_nlink > 1) return true;
    return false;
}


FileBase::~FileBase()
{
}
