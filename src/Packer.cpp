#include "Packer.h"

Packer::Packer(std::string _root_path, std::string _pack_path, const Filter &_filter, bool _verbose)
{
    root_path.assign(_root_path);
    bak_path.assign(_pack_path);
    filter = _filter;
    verbose = _verbose;
}

Packer::~Packer()
{
}

void Packer::fileDfs(FileBase &bak_file, FilePath cur_path)
{
    FileBase file(cur_path); //这里当前工作路径已经改变了，直接使用cur_path就可以
    FileHeader fileHeader = file.getFileHeader(); //得到文件头
    FileType type= file.getFileType(); //获取文件类型
    

    //过滤
    bool flag = filter.check(fileHeader);//调用Filter的check方法
    //check 不通过
    
    //输出检验,调试用
    if(verbose){
        std::cout << cur_path.string();
        if (flag)
            std::cout << "  packed" << std::endl;
        else
            std::cout << "  not packed" << std::endl;
    }
    
    if(!flag){
        //如果是目录文件继续搜索
        if(type==FILE_TYPE_DIR){
            for (const auto &son : std::filesystem::directory_iterator(cur_path)){
                fileDfs(bak_file, son.path());//遍历目录下的所有文件
            }
        }
        return; 
    }

    //处理各类文件
    char buf[MAX_BUFFER_SIZE] = {0};
    bak_file.write((const char *)&fileHeader, sizeof(fileHeader));//对于所有文件，都需要先写入文件头
    if(type == FILE_TYPE_DIR){//目录文件不能直接整个拷贝，因为要特殊处理目录文件下的各种文件类型
        for (const auto &son : std::filesystem::directory_iterator(cur_path))
        { 
            fileDfs(bak_file, son.path());
        }
    }else if(type == FILE_TYPE_REG || type == FILE_TYPE_HARD_LINK || type == FILE_TYPE_SYMBOLIC_LINK){
        //普通文件类型，同时拷贝文件内容

        //硬链接文件类型
        //需要判断硬链接指向的地址是否被备份
        //如果未备份，则存储当前备份路径，并当做普通文件备份处理
        //如果已备份，则将备份文件指向已备份文件，只备份文件头，这里linkName就有用了

        //软链接文件类型，需要判断连接文件是否在备份文件中
        //如果在备份文件中，则创建一个新的软链接指向备份文件中的路径
        //如果不在备份文件中，则不备份

        //这里先不实现，都直接当做普通文件处理
        file.openFile(std::ios::in | std::ios::binary);

        while(file.peek() != EOF){
            file.read(buf, sizeof(buf));
            bak_file.write(buf, file.gcount()); //gcount方法返回上一次读取成功的字符数
        }
        file.close();
        
    }
}


bool Packer::pack()
{
    bak_path += FILE_SUFFIX_PACK;//这里path类应该是继承的string，所以可以直接加
    FileHeader bak_fileHeader = {0}; //设置备份文件的文件头
    strcpy(bak_fileHeader.name, (const char*)bak_path.string().c_str()); //文件名
    bak_fileHeader.fileStat.st_mode = _S_IFREG;//设置为普通文件
    bak_fileHeader.fileStat.st_nlink = 1; //在windows下无意义
    bak_fileHeader.linkType = 0; //文件不存在，不为链接
    
    //创建打包文件
    // print_uid();
    FileBase bak_file(bak_fileHeader);
    if (!bak_file.openFile(std::ios::out | std::ios::binary | std::ios::trunc)){
        // print_uid();
        // std::cerr << "fail to create a backup"<<std::endl;
        perror("Failed to create a backup file");
        return false;
    }
    
    bak_file.writeBackupInfo();//写入备份信息

    // 切换工作目录
    // 所有备份文件的路径都是相对于备份目录下的
    // 解包时备份文件也是相对解包路径
    std::filesystem::current_path(root_path.parent_path());

    //dfs 目录树, 找到所有备份文件
    fileDfs(bak_file, std::filesystem::relative(root_path, root_path.parent_path()));

    bak_file.close();

    return true;
}


bool Packer::unpack(bool restore_fileState)
{
    //判断扩展名
    if (bak_path.extension() != FILE_SUFFIX_PACK) return false;
    // 打开备份文件

    FileBase bak_file(bak_path);
    bak_file.openFile(std::ios::in | std::ios::binary);

    // 获取备份信息
    BackupInfo info = bak_file.readBackupInfo();
    //校验备份信息
    
    //在指定目录下解压
    fs::create_directories(root_path);
    fs::current_path(root_path);

    // 获取文件头
    FileHeader fileHeader = {0}; //用于存储解包文件的文件头
    char buf[MAX_BUFFER_SIZE]; //缓冲区
    while (bak_file.peek() != EOF){
        fileHeader = bak_file.readFileHeader();  //文件头
        FileBase unbak_file(fileHeader); //根据文件头在解压目录下创建文件
        // 普通文件或链接文件需要读取具体内容
        FileType type = unbak_file.getFileType();
         if (type == FILE_TYPE_REG || type == FILE_TYPE_HARD_LINK){
            unbak_file.openFile(std::ios::out | std::ios::binary | std::ios::trunc);//打开解包文件准备写入
            size_t size = unbak_file.getFileSize();
            while (size >= MAX_BUFFER_SIZE)
            {
                bak_file.read(buf, MAX_BUFFER_SIZE);
                unbak_file.write(buf, MAX_BUFFER_SIZE);
                size -= MAX_BUFFER_SIZE;
            }
            if (size)
            {
                bak_file.read(buf, size);
                unbak_file.write(buf, size);
            }
            unbak_file.close();//关闭解包文件
        }

        //恢复文件元数据
        if(restore_fileState){
            unbak_file.restoreFileStat();
        }
    }
    // 注意size的使用
    bak_file.close();//关闭备份文件
    return true;
}
