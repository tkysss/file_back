#include<cstdio>
#include<iostream>

#include "task.h"

#include<openssl/aes.h>

using namespace std;

#include "FileBase.h"
#include "Task.h"
#include "Parser.h"

#include<ctime>
#include<chrono>
#include<iostream>

void test1(){
    //测试能否正常进行文件操作
    string path("A/b.pak");
    FileBase file(path);
    bool flag = file.openFile(std::ios::out | std::ios::binary | std::ios::trunc);
    file.write("123456",6);
    file.close();
    cout << flag << endl;
}




int main(int argc, char **argv)
{
    //测试c++编译版本
    // if (__cplusplus == 201703L) std::cout << "C++17\n";
    // else if (__cplusplus == 201402L) std::cout << "C++14\n";
    // else if (__cplusplus == 201103L) std::cout << "C++11\n";
    // else if (__cplusplus == 199711L) std::cout << "C++98\n";
    // else std::cout << "pre-standard C++\n";
 
    //std::cin.get();  //保留命令窗口
 
    //
    // test1();

    // {
        
    //     fs::current_path("C:/Users/tkysss/Desktop/exp/file_backup/A");
	//     FilePath filePath("B/a.cpp.lnk");
	//     FilePath linkName("B/a.cpp");
	
	//     std::cout<<fs::current_path()<<std::endl;
	//     std::cout<<filePath<<std::endl;
	//     std::cout<<linkName<<std::endl;
	
	
	//  if (!CreateSymbolicLink((LPCSTR)filePath.string().c_str(), (LPCSTR)linkName.string().c_str(), 0))
    // {
    //     DWORD lastError = GetLastError();
    //     if (lastError != ERROR_ALREADY_EXISTS)
    //         std::cout << "Failed to create a symbolic link" << std::endl;
        
        
    // }
	// return 0;
    // }


    //输出测试时间
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "Time: " << std::ctime(&now_c) << std::endl;

     if (argc < 2)
    {
        std::cout << "Usage: filebak [OPTION...]" << std::endl;
        std::cout << "Try 'filebak --help' for more information." << std::endl;
        return -1;
    }

    Parser parser;
    if (!parser.parse(argc, argv))
    {
        parser.errPrint();
        return -1;
    }
    if (parser.flag_help)
        return 0;

    // 备份
    if (parser.flag_backup)
    {
        Task task(parser.str_input, parser.str_output);

        unsigned char mod = 0;
        if (parser.flag_compress)
            mod |= MOD_COMPRESS;
        if (parser.flag_encrypt)
        {
            mod |= MOD_ENCRYPT;
            if (!parser.flag_password)
            {
                std::string pwd1, pwd2;
                std::cout << "Input password: ";
                std::cin >> pwd1;
                std::cout << "Input password again: ";
                std::cin >> pwd2;
                if (pwd1 != pwd2)
                {
                    std::cout << "error: passwords do not match" << std::endl;
                    return -1;
                }

                parser.flag_password = true;
                parser.str_password = pwd2;

                if (!parser.checkPassword())
                    return -1;
            }
        }

        task.setMod(mod);
        task.setVerbose(parser.flag_verbose);
        task.setComment(parser.str_message);

        Filter filter;
        if (parser.flag_path)
            filter.setPathFilter(parser.str_path);
        if (parser.flag_name)
            filter.setNameFilter(parser.str_name);
        if (parser.flag_type)
            filter.setFileType(parser.file_type);
        // if (parser.flag_atime)
        //     filter.setAccessTime(parser.atime_start, parser.atime_end);
        // if (parser.flag_mtime)
        //     filter.setModifyTime(parser.mtime_start, parser.mtime_end);
        // if (parser.flag_ctime)
        //     filter.setChangeTime(parser.ctime_start, parser.ctime_end);
        task.setFilter(filter);

        if (!task.backup(parser.str_password))
            return -1;
        else
            std::cout<<"Done"<<std::endl;
    }

    // 恢复
    if (parser.flag_restore)
    {
        Task task(parser.str_output, parser.str_input);
        if (!task.getBackupInfo())
            return -1;
        if ((task.getBackupMode() & MOD_ENCRYPT) && !parser.flag_password)
        {
            std::cout << "Input password: ";
            std::cin >> parser.str_password;
        }
        task.restoreMetadata(parser.flag_metadata);
        task.setVerbose(parser.flag_verbose);
        if (!task.restore(parser.str_password))
            return -1;
        else
            std::cout<<"Done"<<std::endl;
    }

    // 显示备份文件信息
    if (parser.flag_list)
    {
        BackupInfo info;
        if (Task::getBackupInfo(parser.str_list, info))
        {
            std::string backup_mode = "pack ";
            if (info.mod & MOD_COMPRESS)
                backup_mode += "compress ";
            if (info.mod & MOD_ENCRYPT)
                backup_mode += "encrypt";
            std::cout << "Backup path: " << info.backup_path << std::endl;
            char buf_time[100];
            strftime(buf_time, sizeof(buf_time), "%Y-%m-%d %H:%M:%S", localtime(&info.timestamp));
            std::cout << "Backup Time: " << buf_time << std::endl;
            std::cout << "Backup Mode: " << backup_mode << std::endl;
            std::cout << "Comment: " << info.comment << std::endl;
        }
        else
        {
            return -1;
        }
    }



    return 0;
}