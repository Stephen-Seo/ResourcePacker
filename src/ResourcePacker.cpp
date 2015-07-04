
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

bool RP::checkIfPackfile(const char* name)
{
    std::ifstream ifstream;
    ifstream.imbue(std::locale::classic());
    ifstream.open(name, std::ios::in | std::ios::binary);

    if(!ifstream.good())
    {
        if(ifstream.is_open())
            ifstream.close();
        return false;
    }

    char* data = (char*) malloc(sizeof(char) * 128);
    unsigned short number;

    // check identifier
    ifstream.read(data, 5);
    data[5] = '\0';

    if(std::strcmp(data, RESOURCE_PACKER_IDENTIFIER) != 0 || !ifstream.good())
    {
        ifstream.close();
        free(data);
        return false;
    }

    // check if filesize is greater than header

    // get number of items
    ifstream.read((char*) &number, 2);

    if(!ifstream.good())
    {
        if(ifstream.is_open())
            ifstream.close();
        free(data);
        return false;
    }

    unsigned long long lastLocation;
    // seek through rest of header
    for(unsigned short i = 0; i < number; ++i)
    {
        // location
        ifstream.read((char*) &lastLocation, 8);

        if(!ifstream.good())
        {
            if(ifstream.is_open())
                ifstream.close();
            free(data);
            return false;
        }

        // name
        ifstream.read(data, 128);

        if(!ifstream.good())
        {
            if(ifstream.is_open())
                ifstream.close();
            free(data);
            return false;
        }
    }

    // check if filesize is greater than last location
    ifstream.seekg(lastLocation);
    if(!ifstream.good())
    {
        if(ifstream.is_open())
            ifstream.close();
        free(data);
        return false;
    }

    ifstream.close();
    free(data);
    return true;
}

bool RP::createPackfile(std::list<std::string> files, std::string packfileName)
{
    // fail if file exists with same filename
    if(checkIfFileExists(packfileName.c_str()))
        return false;

    PackInfo info;
    info.items = files.size();

    char* data = (char*) malloc(sizeof(char) * RW_BYTE_RATE);
    unsigned int size;

    // get file sizes
    for(auto iter = files.begin(); iter != files.end(); ++iter)
    {
        // fail if filename is greater than 127 characters long
        if(getNameFromPath(*iter).size() > 127)
            return false;

        size = 0;
        info.names.push_back(getNameFromPath(*iter));

        std::ifstream ifstream;
        ifstream.imbue(std::locale::classic());
        ifstream.open(*iter, std::ios::in | std::ios::binary);

        while(!ifstream.eof())
        {
            ifstream.read(data, RW_BYTE_RATE);
            size += ifstream.gcount();
#ifndef NDEBUG
            std::cout << *iter << " partialSize: " << size << '\n';
#endif
        }

#ifndef NDEBUG
        std::cout << *iter << " size: " << size << '\n';
#endif

        info.locations.push_back(size);

        ifstream.close();
    }

    // calculate header size
    unsigned int header_size = 5 + 2 + info.items * 8 + info.items * 128;
#ifndef NDEBUG
    std::cout << "HEADER_SIZE = " << header_size << '\n';
#endif

    // calculate/set locations
    unsigned long long temp = 0;
    unsigned long long location = header_size;
    for(int i = 0; i < info.items; ++i)
    {
        location += temp;
        temp = info.locations[i];
        info.locations[i] = location;
#ifndef NDEBUG
        std::cout << info.names[i] << " location calculated at " << location << '\n';
#endif
    }
    

    std::ofstream ofstream;
    ofstream.imbue(std::locale::classic());
    ofstream.open(packfileName, std::ios_base::out | std::ios_base::binary);

    // write identifier
    std::strncpy(data, RESOURCE_PACKER_IDENTIFIER, 5);
    ofstream.write(data, 5);

    // write number of items
    ofstream.write((char*) &info.items, 2);

    // write items table of contents
    for(int i = 0; i < info.items; ++i)
    {
        // write location of item
        ofstream.write((char*) &info.locations[i], 8);

        // write name of item
        // offset by 1 due to null-terminated c string
        temp = info.names[i].size() + 1;

        ofstream.write(info.names[i].c_str(), temp);

        if(temp != 128)
        {
            char* filler = (char*) calloc(1, sizeof(char) * (128 - temp));
            ofstream.write(filler, 128 - temp);
            free(filler);
        }
    }

    // write files as pack content
    for(auto iter = files.begin(); iter != files.end(); ++iter)
    {
        std::ifstream ifstream;
        ifstream.imbue(std::locale::classic());
        ifstream.open(*iter, std::ios_base::in | std::ios_base::binary);

        while(ifstream.good())
        {
            ifstream.read(data, RW_BYTE_RATE);
            temp = ifstream.gcount();
            ofstream.write(data, temp);
        }

        ifstream.close();
    }

    ofstream.close();

    free(data);

    return true;
}

bool RP::readPackfileInfo(std::string packfileName, PackInfo& packInfo)
{
    if(!checkIfFile(packfileName.c_str()) || !checkIfPackfile(packfileName.c_str()))
        return false;

    std::ifstream ifstream;
    ifstream.imbue(std::locale::classic());
    ifstream.open(packfileName, std::ios::in | std::ios::binary);

    // skip identifier
    char* tempData = (char*) malloc(sizeof(char) * 5);
    ifstream.read(tempData, 5);
    free(tempData);

    // get number of items
    ifstream.read((char*) &packInfo.items, 2);

    packInfo.locations.assign(packInfo.items, 0);
    packInfo.names.assign(packInfo.items, "");

    char data;
    int x;
    bool nullReached;

    for(unsigned short i = 0; i < packInfo.items; ++i)
    {
        // read location
        ifstream.read((char*) &packInfo.locations[i], 8);

        // read name
        x = 0;
        nullReached = false;
        do
        {
            ifstream.read(&data, 1);
            if(data == '\0')
                nullReached |= true;
            if(!nullReached)
                packInfo.names[i] += data;
            ++x;
        } while (x < 128);
    }

    return true;
}

bool RP::getFileData(std::unique_ptr<char[]>& dataPtr, unsigned long long& size, std::string packfile, std::string filename)
{
    if(!checkIfPackfile(packfile.c_str()))
        return false;

    std::ifstream ifstream;
    ifstream.imbue(std::locale::classic());
    ifstream.open(packfile, std::ios::in | std::ios::binary);

    char* data = (char*) malloc(sizeof(char) * 128);

    ifstream.read(data, 5);

    unsigned short items;
    unsigned long long location;
    unsigned long long nextLocation;
    bool found = false;
    bool nextExists = false;

    ifstream.read((char*) &items, 2);

    // get file location from header
    for(unsigned short i = 0; i < items; ++i)
    {
        ifstream.read((char*) &location, 8);

        ifstream.read(data, 128);

        if(std::strcmp(data, filename.c_str()) == 0)
        {
            found = true;
            if(i + 1 < items)
            {
                ifstream.read((char*) &nextLocation, 8);
                nextExists = true;
            }
            break;
        }
    }

    if(!found || !ifstream.good())
    {
        ifstream.close();
        free(data);
        return false;
    }

    // get file size
    size = 0;

    if(nextExists)
    {
        size = nextLocation - location;
    }
    else
    {
        ifstream.seekg(location);
        while(ifstream.good())
        {
            ifstream.read(data, 128);
            size += ifstream.gcount();
        }
    }
    
#ifndef NDEBUG
    std::cout << "size of file: " << size << '\n' << std::flush;
#endif

    ifstream.clear();

    // get file data
    std::unique_ptr<char[]> fileData(new char[size]);
    ifstream.seekg(location);
    ifstream.read(fileData.get(), size);

    dataPtr = std::move(fileData);

    ifstream.close();

    free(data);
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
