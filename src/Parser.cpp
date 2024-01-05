#include "Parser.h"

Parser::Parser() : options("filebak", "A data backup software for windows")
{
    //基本命令
    //-b 备份 flag_backup 
    //-r 恢复 flag_restore
    //-l 查看 str_list
    //-h 帮助文档 flag_help
    //基本参数
    //-i 输入文件路径 str_input
    //-o 输出文件路径 str_output 
    //-p 密码 str_password
    //-v 是否输出执行过程 flag_verbose
    options.set_width(ARG_HELP_WIDTH).add_options()
    ("b,backup", "backup", cxxopts::value(flag_backup))
    ("r,restore", "restore", cxxopts::value(flag_restore))
    ("l,list", "list the file", cxxopts::value(str_list))
    ("v,verbose", "print the backup process", cxxopts::value(flag_verbose))
    ("i,input", "input path location", cxxopts::value(str_input))
    ("o,output", "output path location", cxxopts::value(str_output))
    ("p,password", "input the password", cxxopts::value(str_password))
    ("h,help", "help document", cxxopts::value(flag_help));


    //备份模式
    //-c 压缩文件 flag_compress
    //-e 加密文件 flag_encrypt
    //-path 过滤路径正则表达式 str_path
    //-type 过滤文件类型 str_type (reg dir link pip)
    //-name 过滤文件名正则表达式 str_name 
    //-atime 文件的访问时间区间 str_atime
    //-mtime 修改时间区间 str_mtime
    //-ctime 改变时间区间 str_ctime
    //-m 备注信息 -str_message 
    options.set_width(ARG_HELP_WIDTH).add_options("Backup")
    ("c,compress", "compress when backup", cxxopts::value(flag_compress))
    ("e,encrypt", "encrypt when backup", cxxopts::value(flag_encrypt))
    ("path", "filter path: regular expression", cxxopts::value(str_path))
    ("type", "filter type: r regular file,d directory,s soft link,h hard link,p pipe", cxxopts::value(str_type))
    ("name", "filter name: regular expression", cxxopts::value(str_name))
    ("atime", "filter access time, eg.\"2000-11-11 23:20:21 2022-10-11 20:10:51\"", cxxopts::value(str_atime))
    ("mtime", "filter modify time", cxxopts::value(str_mtime))
    ("ctime", "filter change time", cxxopts::value(str_ctime))
    ("m,message", "add backup comments", cxxopts::value(str_message));

    //-a 文件元数据在解包时是否恢复 flag_metedata
    options.set_width(ARG_HELP_WIDTH).add_options("Restore")
    ("a,metadata", "restore the metedata", cxxopts::value(flag_metadata));
}

Parser::~Parser()
{
}

bool Parser::parse(int argc, char **argv)
{
    try
    {
        //返回cxxopts解析的结果
        result = options.parse(argc, argv);
    }
    catch (cxxopts::OptionException err)
    {
        std::cout << err.what() << std::endl;
        return false;
    }

    flag_list = result.count("list");
    flag_input = result.count("input");
    flag_output = result.count("output");
    flag_password = result.count("password");
    flag_path = result.count("path");
    flag_type = result.count("type");
    flag_name = result.count("name");
    flag_atime = result.count("atime");
    flag_mtime = result.count("mtime");
    flag_ctime = result.count("ctime");
    flag_message = result.count("message");

    //输出帮助文档
    if (flag_help)
    {
        std::cout << options.help() << std::endl;
        return true;
    }

    
    // 判断参数格式是否合法
    // 暂时不实现
    // bool format_correct = true;
    // if (flag_backup + flag_restore + flag_list != 1)
    // {
    //     std::cout << "error: invalid parameter" << std::endl;
    //     return false;
    // }
    // if (flag_backup)
    //     format_correct &= CheckArgsBackup();
    // if (flag_restore)
    //     format_correct &= CheckArgsRestore();
    // if (flag_list)
    //     format_correct &= CheckArgsList();
    // if (flag_atime)
    //     format_correct &= ConvertTime(str_atime, atime_start, atime_end);
    // if (flag_mtime)
    //     format_correct &= ConvertTime(str_mtime, mtime_start, mtime_end);
    // if (flag_ctime)
    //     format_correct &= ConvertTime(str_ctime, ctime_start, ctime_end);
    // if (flag_backup && flag_password)
    //     format_correct &= CheckPassword();
    // format_correct &= CheckRegExp();
    // if (!format_correct)
    //     return false;

    //处理指定的文件类型文件类型 flie_type
    if (flag_type)
    {
        file_type = 0;
        if (str_type.find("r") != std::string::npos)
            file_type |= FILE_TYPE_REG;
        if (str_type.find("d") != std::string::npos)
            file_type |= FILE_TYPE_DIR;
        if (str_type.find("s") != std::string::npos)
            file_type |= FILE_TYPE_SYMBOLIC_LINK;
        if (str_type.find("h") != std::string::npos)
            file_type |= FILE_TYPE_HARD_LINK;
        if (str_type.find("p") != std::string::npos)
            file_type |= FILE_TYPE_FIFO;
    }
    return true;
}


void Parser::errPrint()
{
    std::cout << "Try 'filebak --help' for more information." << std::endl;
}

bool Parser::checkPassword()
{
    if (str_password.length() < 6)
    {
        std::cout << "error: password must be at least 6 characters in length" << std::endl;
        return false;
    }
    return true;
}

bool Parser::checkRegExp()
{
    try
    {
        if (flag_path)
            std::regex reg(str_path);
        if (flag_name)
            std::regex reg(str_name);
    }
    catch (const std::exception &e)
    {
        std::cout << "error: invalid regular expression: ";
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}
