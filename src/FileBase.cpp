#include "FileBase.h"


void print_uid()
{
    DWORD dwSize = MAX_PATH;
    char szUsername[MAX_PATH];
    
    if (GetUserNameA(szUsername, &dwSize)) {
        std::cout << "now ID: " << szUsername << std::endl;
    } else {
        std::cout << "not available user ID" << std::endl;
    }

}

FileBase::FileBase(FileHeader _fileHeader)
{
    this -> fileHeader = _fileHeader;
    filePath.assign(_fileHeader.name);


    //注意这里没有处理文件已经存在的情况
    //这里字符集好像出了点问题
    //似乎要转成wstr
    // print_uid();
    //这里似乎都只会返回普通文件，因为文件还未创建
    //这里因为文件还没有被创建，所以判断文件类型的时候用st_mode比较好
    switch (getFileType())
    {
    case FILE_TYPE_REG:
        //这里只创建父目录
        // if(CreateFile((const char*)filePath.parent_path().string().c_str(),GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)==INVALID_HANDLE_VALUE)
        // if (!CreateDirectory((LPCSTR)filePath.parent_path().string().c_str(), NULL)){
        //     DWORD lastError = GetLastError();
        //     if (lastError != ERROR_ALREADY_EXISTS) std::cout << "failed to create a parent dir" << std::endl;
        // }
        // try {
            // 创建目录及其父目录
            create_directories(filePath.parent_path());
            // std::cout << "Directory created: " << dirPath.string() << std::endl;
        // } catch (const filesystem_error& ex) {
        //     std::cerr << "Failed to create directory: " << ex.what() << std::endl;
        // }
        break;
    case FILE_TYPE_DIR:
        // if(!CreateDirectory((LPCSTR)filePath.string().c_str(), NULL)) {
        //     DWORD lastError = GetLastError();
        //     if (lastError != ERROR_ALREADY_EXISTS) std::cout << "failed to create a dir" << std::endl;
        // }
        // try {
            // 创建目录及其父目录
            create_directories(filePath);
            // std::cout << "Directory created: " << dirPath.string() << std::endl;
        // } catch (const filesystem_error& ex) {
        //     std::cerr << "Failed to create directory: " << ex.what() << std::endl;
        // }
        break;
    case FILE_TYPE_HARD_LINK:
        if (!CreateHardLink((LPCSTR)filePath.string().c_str(), (LPCSTR)fileHeader.linkName, NULL)) std::cout << "failed to create hard link file" << std::endl;
        // if (!CreateHardLink((const char*)filePath.string().c_str(), wideLinkName.c_str(), NULL)) std::cout << "failed to create hard link file" << std::endl;
        break;
    case FILE_TYPE_SYMBOLIC_LINK:
        // std::wstring wideFilePath(filePath.string().begin(), filePath.string().end());  // 将文件路径转换为宽字符
        // LPCWSTR wideFilePathPtr = wideFilePath.c_str();  // 获取宽字符路径的 LPCWSTR 指针

        //调试路径源
        std::cout<<fileHeader.linkName << std::endl;
        std::cout<<fs::current_path() << std::endl;
        std::cout<<filePath.string() << std::endl;

        if (!CreateSymbolicLink((LPCSTR)filePath.string().c_str() , (LPCSTR)fileHeader.linkName, 1)) {
            DWORD lastError = GetLastError();
            if (lastError != ERROR_ALREADY_EXISTS) std::cout << "failed to create a symbolic link" << std::endl;
        }
        break;
    case FILE_TYPE_PIPE:
        if (!CreateNamedPipe((LPCSTR)filePath.string().c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 1024, 1024, NMPWAIT_USE_DEFAULT_WAIT, NULL))
        std::cout << "failed to create pipe file" << std::endl;
        break;
    default:
        break;
    }
}

FileBase::FileBase(FilePath _filePath)
{
    filePath = _filePath;

    //在windows下似乎这个不咋好用，只能判断部分文件类型
    //对剩下的类型打了补丁
    strcpy(fileHeader.name, (const char*)filePath.string().c_str());
    if (_stat((const char*)filePath.string().c_str(), &(fileHeader.fileStat))  != 0) { //获取文件元数据，为0则表示返回正常
        // std::cerr << "falied to get file information" << std::endl;
    }
    fileHeader.linkType = -1; //未指定链接类型
    FileType fileType = getFileType();
    
    //处理软链接路径，用于判断打包范围内是否存在链接对象
    if(fileType==FILE_TYPE_HARD_LINK){
        fileHeader.linkType = 1;
        strcpy(fileHeader.linkName, (const char*)filePath.string().c_str()); //如果是硬链接直接记录当前文件路径
    }else if(fileType==FILE_TYPE_SYMBOLIC_LINK){
        fileHeader.linkType = 2;
        FilePath linkPath(filePath);
        // FilePath targetPath = fs::read_symlink(linkPath);
        linkPath.replace_extension(""); //直接拿掉.lnk后缀
        strcpy(fileHeader.linkName, (const char*)linkPath.string().c_str());  
        //如果是软链接获取并记录连接文件路径
    }else fileHeader.linkType = 0;
}

bool FileBase::isHardLink()
{   
    //如果已指定链接类型
    if(fileHeader.linkType!=-1)return fileHeader.linkType==1;

    //否则默认文件已存在
    // 在windows下判定硬链接文件
    // puts("hh1");
    BY_HANDLE_FILE_INFORMATION fileInformation;
    HANDLE fileHandle = CreateFile((LPCSTR)filePath.string().c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {//无此文件时需要返回false
        return false;
        // puts("hh");
    }
    bool result = GetFileInformationByHandle(fileHandle, &fileInformation) && fileInformation.nNumberOfLinks > 1;
    CloseHandle(fileHandle);
    // puts("01010");
    // std::cout<<result<<std::endl;
    return result;
    // return fileHeader.fileStat.st_nlink > 1;
}

bool FileBase::isSymbolicLink()
{
    //如果已指定链接类型
    if(fileHeader.linkType!=-1)return fileHeader.linkType==2;
    
    //否则默认文件已存在
    //在windows环境下判断软链接文件
    // DWORD fileAttributes = GetFileAttributes((const char*)filePath.string().c_str());

    // int flag1 = (fileAttributes != INVALID_FILE_ATTRIBUTES);
    // int flag2 = (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
    // if (flag1&&flag2) std::cout<<"haha"<<std::endl;
    // return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
    return filePath.extension()==".lnk";
    //无该文件时会返回false
}

FileBase::~FileBase()
{
}

FileType FileBase::getFileType()
{
    // if (isHardLink()){
    //     return FILE_TYPE_HARD_LINK;
    // } else if(isSymbolicLink()){
    //     return FILE_TYPE_SYMBOLIC_LINK;
    // }
    // struct _stat fileStat = fileHeader.fileStat;
    //   if (fileStat.st_mode & _S_IFREG) {
    //         return FILE_TYPE_REG;
    //     } else if (fileStat.st_mode & _S_IFDIR) {
    //         return FILE_TYPE_DIR;
    //     } else if (fileStat.st_mode & _S_IFIFO) {
    //         return FILE_TYPE_PIPE;
    //     } else {
    //         return FILE_TYPE_OTHER;
    //     }
    // if(isHardLink()) return FILE_TYPE_HARD_LINK;
    // else if(isSymbolicLink()) return FILE_TYPE_SYMBOLIC_LINK;
    
    //链接文件都当普通文件处理
    if(isHardLink()) return FILE_TYPE_REG;
    else if(isSymbolicLink()) return FILE_TYPE_REG;
    return getFileType(fileHeader);
}

FileType FileBase::getFileType(const FileHeader &_fileHeader)
{
    // FilePath path;
    // path.assign(_fileHeader.name);
    // FileBase file(path);
    // return file.getFileType();
    switch (_fileHeader.fileStat.st_mode & _S_IFMT)
    {
    case _S_IFREG:
        return FILE_TYPE_REG;
        break;
    case _S_IFDIR:
        return FILE_TYPE_DIR;
        break;
    case _S_IFIFO:
        return FILE_TYPE_FIFO;
        break;
    default:
        return FILE_TYPE_OTHER;
        break;
    }
}

bool FileBase::openFile(std::ios_base::openmode _mode)
{
    this->open(filePath.string(), _mode);
    return is_open();
}

BackupInfo FileBase::readBackupInfo()
{
    BackupInfo info;
    this->read((char*)&info, sizeof(info));
    return info;
}

void FileBase::writeBackupInfo(BackupInfo _backupInfo)
{
    uint32_t checksum =  _backupInfo.calcCheckSum();
    checksum = (~checksum) + 1;
    _backupInfo.checksum = checksum;
    this->write((char*)&_backupInfo, sizeof(_backupInfo));
}

void FileBase::writeBackupInfo()
{
    BackupInfo info = {0};
    writeBackupInfo(info);
}

FileHeader FileBase::readFileHeader()
{
    FileHeader fileHeader;
    this->read((char*)&fileHeader, sizeof(fileHeader));
    return fileHeader;
}

void FileBase::writeFileHeader(FileHeader _fileHeader)
{
    this->write((char*)&_fileHeader, sizeof(_fileHeader));
}

void FileBase::restoreFileStat()
{
    // struct _stat  *pstat = &(fileHeader.fileStat);
    // windows 下将解包文件的元数据还原成文件头的元数据
}

size_t FileBase::getFileSize()
{
    return fileHeader.fileStat.st_size;
}

FileHeader FileBase::getFileHeader()
{
    return this->fileHeader;
}

