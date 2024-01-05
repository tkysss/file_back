#include "Task.h"

Task::Task(std::string _src_path, std::string _bak_path)
{
    //设置任务路径
    if(_src_path.length() == 0) _src_path = ".";
    src_path = std::filesystem::absolute(_src_path);
    bak_path = std::filesystem::absolute(_bak_path);

    //设置info备份信息
    memset(&info, 0, sizeof(info));
    info.mod = MOD_COMPRESS | MOD_ENCRYPT; //这里打包和解包通用
    info.timestamp = std::time(nullptr); //时间戳
    memcpy(info.backup_path, _src_path.c_str(), std::min(_src_path.length(), sizeof(info.backup_path)));
    
    //默认不还原和输出
    restore_metadata = false;
    verbose = true;
}

Task::~Task()
{
}

void Task::setVerbose(bool _verbose)
{
    //设置输出过程的代码
    verbose = _verbose;
}



void Task::setComment(std::string _comment)
{
    // 设置备份注释的代码
    memset(&(info.comment), 0, sizeof(info.comment));
    memcpy(info.comment, _comment.c_str(), std::min(_comment.length(), sizeof(info.comment)));
}

void Task::setMod(unsigned char _mod)
{
    // 设置备份模式的代码
    info.mod = _mod;
}

void Task::setFilter(const Filter &_filter)
{
    // 设置备份过滤器的代码
    filter = _filter;
}

void Task::restoreMetadata(bool _restore_metadata)
{
    // 设置是否恢复元数据的代码
    restore_metadata = _restore_metadata;
}

bool Task::getBackupInfo()
{
    // 获取备份信息的代码
    return Task::getBackupInfo(this->bak_path.string(), this->info);
}

unsigned char Task::getBackupMode()
{
    // 获取备份模式的代码
    return info.mod;
}

bool Task::getBackupInfo(std::string _file_path, BackupInfo &_info)
{
    // 获取备份文件基本信息的代码
    FileBase file(_file_path);
    if(file.openFile(std::ios::in | std::ios::binary)){
        _info = file.readBackupInfo();

        bool flag = true;
        if(_info.calcCheckSum()){
            std::cout << "error: checksum failed: " << _file_path << std::endl;
            flag = false;
        }
        file.close();
        return flag;
    }
    std::cout << "error: failed to open file: " << _file_path << std::endl;
    return false;
}

bool Task::backup(std::string password)
{
    // 实现备份功能的代码
    
    //判断路径是否存在
    if (!std::filesystem::exists(src_path))
    {
        std::cout << "error: no such file or directory: " << src_path.string() << std::endl;
        return false;
    }

    // 判断文件名是否符合要求
    // std::string name = bak_path.filename().string();
    // std::regex reg("^[^\\\/:\*\?\"<>\|]{1,260}(?<!\.)$");
    // /*Windows合法文件名
    // 1.不能包含以下字符 \/:*?"<>|
    // 2.文件名长度不能超过 260 个字符（路径名长度不能超过 248 个字符
    // 3.不能以点号 "." 结尾*/
    // if (!std::regex_match(name, reg))
    // {
    //     std::cout << "error: invalid file name: " << name << std::endl;
    //     return false;
    // }

    //打包
    if (verbose)
        std::cout << "PACKING..." << std::endl;
    //打包类
    Packer packer(src_path.string(), bak_path.string(), filter, verbose);
    //调用pack方法
    if (!packer.pack())
    {
        std::cout << "error: failed to pack file" << std::endl;
        return false;
    }
    bak_path += FILE_SUFFIX_PACK;//目标文件变为打包后的文件
    
    //压缩
    if (info.mod & MOD_COMPRESS)
    {
        if (verbose)
        std::cout << "COMPRESSING..." << std::endl;

        Compressor compressor(bak_path.string());
        if (!compressor.compress())
        {
            std::cout << "error: failed to compress file" << std::endl;
            return false;
        }

        std::filesystem::remove_all(bak_path);//删除中间文件
        bak_path += FILE_SUFFIX_COMPRESS;//目标文件变为压缩后的文件
    }

    //加密
    if (info.mod & MOD_ENCRYPT)
    {
        if (verbose)
            std::cout << "ENCRYPTING..." << std::endl;

        Aes aes(bak_path.string(), password);
        if (!aes.encrypt())
        {
            std::cout << "error: failed to encrpy file" << std::endl;
            return false;
        }

        std::filesystem::remove_all(bak_path);//删除中间文件
        bak_path += FILE_SUFFIX_ENCRYPT;//目标文件变为加密后的文件
    }

    //写入备份信息
    //备份信息的地址在*file, 在packer中已经被初始化
    FileBase file(bak_path);
    file.openFile(std::ios::in | std::ios::out | std::ios::binary);
    file.writeBackupInfo(info);
    file.close();

    return true;
}

bool Task::restore(std::string password)
{
    // 实现恢复功能的代码

    // 判断路径是否存在
    if (!std::filesystem::exists(bak_path))
    {
        std::cout << "error: no such file or directory: " << src_path.string() << std::endl;
        return false;
    }
    
    // 解密
    if (info.mod & MOD_ENCRYPT)
    {
        if (verbose)
            std::cout << "DECRYPTING..." << std::endl;
        
        Aes aes(bak_path.string(), password);
        int status = aes.decrypt();
        if (status == -2)
        {
            std::cout << "error: failed to decrypt file" << std::endl;
            return false;
        }
        else if (status == -1)
        {
            std::cout << "error: wrong password" << std::endl;
            return false;
        }
        bak_path.replace_extension("");//定位解密后的文件
    }

    // 解压
    if (info.mod & MOD_COMPRESS)
    {
        if (verbose)
            std::cout << "DECOMPRESSING..." << std::endl;
        
        Compressor compressor(bak_path.string());
        if (!compressor.decompress())
        {
            std::cout << "error: failed to decompress file" << std::endl;
            return false;
        }
        if (info.mod & MOD_ENCRYPT) std::filesystem::remove_all(bak_path); //删除中间文件
        bak_path.replace_extension("");//定位解压后的文件
    }

    // 解包
    if (verbose)
        std::cout << "UNPACKING..." << std::endl;
    Packer packer(src_path.string(), bak_path.string(), filter, verbose);
    if (!packer.unpack(restore_metadata))
    {
        std::cout << "error: failed to unpack file" << std::endl;
        return false;
    }
    if (info.mod & MOD_COMPRESS) std::filesystem::remove_all(bak_path);//删除中间文件

    return true;
}