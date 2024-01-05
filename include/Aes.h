#ifndef AES_H
#define AES_H

#include "FileBase.h"
#include <openssl/aes.h>
#include <openssl/md5.h>

class Aes
{
private:
    unsigned char key[MD5_DIGEST_LENGTH];
    std::filesystem::path file_path;

public:
    Aes(std::string _file_path, std::string _password);
    ~Aes();

    bool encrypt();

    //解密
    //0成功 -1密码错误 -2文件错误
    int decrypt(); 
};

#endif