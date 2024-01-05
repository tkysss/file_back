#include "Filter.h"

Filter::Filter()
{
    type = 0;
}

Filter::~Filter()
{
}

//路径匹配
void Filter::setPathFilter(std::string reg_path_)
{
    reg_path = reg_path_;
    type |= FILTER_FILE_PATH;
}

//文件名匹配
void Filter::setNameFilter(std::string reg_name_)
{
    reg_name = reg_name_;
    type |= FILTER_FILE_NAME;
}

//文件类型匹配
void Filter::setFileType(FileType file_type_)
{
    file_type = file_type_;
    type |= FILTER_FILE_TYPE;
}

bool Filter::check(const FileHeader &file_header_)
{
    FileType cur_file_type = FileBase::getFileType(file_header_);
    // 路径匹配
    if (type & FILTER_FILE_PATH)
    {
        std::regex reg(reg_path);
        std::string dir_path = file_header_.name;
        if (cur_file_type == FILE_TYPE_DIR)
        {
            std::filesystem::path file_path(file_header_.name);
            dir_path = file_path.parent_path().string();
        }
        if (!std::regex_search(dir_path, reg))
        {
            return false;
        }
    }

    // 文件名匹配
    if ((type & FILTER_FILE_NAME) && (cur_file_type != FILE_TYPE_DIR))
    {
        std::regex reg(reg_name);
        std::filesystem::path file_path(file_header_.name);
        if (!std::regex_search(file_path.filename().string(), reg))
        {
            return false;
        }
    }

    // 类型匹配
    if ((type & FILTER_FILE_TYPE) && !(cur_file_type & file_type))
    {
        return false;
    }

    return true;
}