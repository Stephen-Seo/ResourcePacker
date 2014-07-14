
#include "ResourcePacker.hpp"

bool RP::checkIfFile(const char* name)
{
    // check if opens
    std::ifstream ifstream;
    ifstream.open(name);

    bool success = ifstream.good();

    if(ifstream.is_open())
        ifstream.close();

    if(!success)
        return false;

    // Check if directory
#if defined(_WIN32)
    DWORD fileType = GetFileAttributes(name);
    if(fileType == FILE_ATTRIBUTE_DIRECTORY)
        return false;
#elif defined(__WIN32__)
    DWORD fileType = GetFileAttributes(name);
    if(fileType == FILE_ATTRIBUTE_DIRECTORY)
        return false;
#elif defined(__APPLE__)
    struct stat fileInfo;
    if(stat(name, &fileInfo) != 0)
        return false;

    if(S_IFDIR == fileInfo.st_mode)
        return false;
#else
    struct stat fileInfo;
    if(stat(name, &fileInfo) != 0)
        return false;

    if(S_ISDIR(fileInfo.st_mode))
        return false;
#endif

    return true;
}

bool RP::checkIfFileExists(const char* name)
{
    std::ifstream ifstream;
    ifstream.open(name);

    bool success = ifstream.good();

    if(ifstream.is_open())
        ifstream.close();

    return success;
}

bool RP::checkIfValidFilename(std::string name)
{
    return name.find_first_not_of(VALID_FILENAME_CHARS) == std::string::npos;
}

bool RP::createPackfile(std::list<std::string> files, std::string packfileName)
{
    if(checkIfFileExists(packfileName.c_str()))
        return false;

    PackInfo info;
    info.items = files.size();

    std::ofstream ofstream;
    ofstream.open(packfileName + ".temp", std::ios_base::out | std::ios_base::binary);

    for(auto iter = files.begin(); iter != files.end(); ++iter)
    {
        info.names.push_back(getNameFromPath(*iter));
        info.locations.push_back(ofstream.tellp());

        std::ifstream ifstream;
        ifstream.open(*iter, std::ios_base::in | std::ios_base::binary);

        char* data = (char*) malloc(sizeof(char) * 255);
        while(!ifstream.eof())
        {
            ifstream.read(data, 255);
            int count = ifstream.gcount();
            if(count == 0)
                break;
            ofstream.write(data, count);
        }
        free(data);

        ifstream.close();
    }

    ofstream.close();
    return true;
}

std::string RP::getNameFromPath(std::string path)
{
    std::string delimeter;
#if defined(_WIN32)
    delimeter = "\\";
#elif defined(__WIN32__)
    delimeter = "\\";
#else
    delimeter = "/";
#endif
    while(path.substr(path.size() - 1, 1) == delimeter)
    {
        path = path.substr(0, path.size() - 1);
    }

    int pos = path.rfind(delimeter);

    return path.substr(pos + 1, path.size() - pos - 1);
}
