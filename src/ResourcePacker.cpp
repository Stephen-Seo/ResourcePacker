
#include "ResourcePacker.hpp"
#include "ResourcePacker_Internal.hpp"

bool RP::isLittleEndian()
{
    union
    {
        std::uint32_t i;
        char c[4];
    } bint = { 0x01020304 };

    return bint.c[0] == 0x4;
}

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

    char* data = (char*) malloc(sizeof(char) * 127);
    std::uint16_t number;

    // check identifier
    ifstream.read(data, 5);

    if(std::memcmp(data, RESOURCE_PACKER_IDENTIFIER, 5) != 0 || !ifstream.good())
    {
        ifstream.close();
        free(data);
        return false;
    }

    // check if filesize is greater than header

    // get number of items
    number = readLittleEndian<std::uint16_t>(ifstream);

    if(!ifstream.good())
    {
        if(ifstream.is_open())
            ifstream.close();
        free(data);
        return false;
    }

    std::uint64_t lastLocation = 0;
    // seek through rest of header
    for(std::uint16_t i = 0; i < number; ++i)
    {
        // location
        lastLocation = readLittleEndian<std::uint64_t>(ifstream);

        if(!ifstream.good())
        {
            if(ifstream.is_open())
                ifstream.close();
            free(data);
            return false;
        }

        // name
        ifstream.read(data, 127);

        if(!ifstream.good())
        {
            if(ifstream.is_open())
                ifstream.close();
            free(data);
            return false;
        }
    }

    // Check if no items in packfile, invalid if true
    if(lastLocation == 0)
    {
        if(ifstream.is_open())
        {
            ifstream.close();
        }
        free(data);
        return false;
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

bool RP::checkIfPackfileFromMemory(const char* data, std::uint64_t size)
{
    std::uint64_t index = 0;
    // first 5 is identifier
    if(std::memcmp(data, RESOURCE_PACKER_IDENTIFIER, 5) != 0)
    {
        return false;
    }
    index += 5;

    // next 2 is number of items
    std::uint16_t number = *((std::uint16_t*)(data + index));
    index += 2;

    // in header, per item
    std::uint64_t lastLocation;
    for(std::uint16_t i = 0; i < number; ++i)
    {
        // last location std::uint64_t
        lastLocation = *((std::uint64_t*)(data + index));
        index += 8;
        // name in 127 bytes
        index += 127;
    }

    if(size < lastLocation)
    {
        return false;
    }

    return true;
}

bool RP::createPackfile(
    std::list<std::string> files, 
    std::string packfileName, 
    bool overwrite,
    bool stripPath,
    char delimeter)
{
    // fail if file exists with same filename
    if(checkIfFileExists(packfileName.c_str()) && !overwrite)
        return false;

    PackInfo info;
    info.items = files.size();

    char* data = (char*) malloc(sizeof(char) * RW_BYTE_RATE);
    std::uint32_t size;

    // get file sizes
    for(auto iter = files.begin(); iter != files.end(); ++iter)
    {
        // fail if filename is greater than 126 characters long
        if((stripPath && getNameFromPath(*iter, delimeter).size() > 126)
            || (!stripPath && iter->size() > 126))
            return false;

        size = 0;
        if(stripPath)
        {
            info.names.push_back(getNameFromPath(*iter, delimeter));
        }
        else
        {
            info.names.push_back(*iter);
        }

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
    std::uint32_t header_size = 5 + 2 + info.items * 8 + info.items * 127;
#ifndef NDEBUG
    std::cout << "HEADER_SIZE = " << header_size << '\n';
#endif

    // calculate/set locations
    std::uint64_t temp = 0;
    std::uint64_t location = header_size;
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
    std::memcpy(data, RESOURCE_PACKER_IDENTIFIER, 5);
    ofstream.write(data, 5);

    // write number of items
    writeLittleEndian(ofstream, info.items);

    // write items table of contents
    for(int i = 0; i < info.items; ++i)
    {
        // write location of item
        writeLittleEndian(ofstream, info.locations[i]);

        // write name of item
        // offset by 1 due to null-terminated c string
        temp = info.names[i].size() + 1;

        ofstream.write(info.names[i].c_str(), temp);

        if(temp != 127)
        {
            char* filler = (char*) calloc(1, sizeof(char) * (127 - temp));
            ofstream.write(filler, 127 - temp);
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
    packInfo.items = readLittleEndian<std::uint16_t>(ifstream);

    packInfo.locations.assign(packInfo.items, 0);
    packInfo.names.assign(packInfo.items, "");

    char data;
    int x;
    bool nullReached;

    for(std::uint16_t i = 0; i < packInfo.items; ++i)
    {
        // read location
        packInfo.locations[i] = readLittleEndian<std::uint64_t>(ifstream);

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
        } while (x < 127);
    }

    return true;
}

bool RP::getFileData(std::unique_ptr<char[]>& dataPtr, std::uint64_t& size, std::string packfile, std::string filename)
{
    if(!checkIfPackfile(packfile.c_str()))
        return false;

    std::ifstream ifstream;
    ifstream.imbue(std::locale::classic());
    ifstream.open(packfile, std::ios::in | std::ios::binary);

    char* data = (char*) malloc(sizeof(char) * 127);

    ifstream.read(data, 5);

    std::uint16_t items;
    std::uint64_t location;
    std::uint64_t nextLocation;
    bool found = false;
    bool nextExists = false;

    items = readLittleEndian<std::uint16_t>(ifstream);

    // get file location from header
    for(std::uint16_t i = 0; i < items; ++i)
    {
        location = readLittleEndian<std::uint64_t>(ifstream);

        ifstream.read(data, 127);

        if(std::strcmp(data, filename.c_str()) == 0)
        {
            found = true;
            if(i + 1 < items)
            {
                nextLocation = readLittleEndian<std::uint64_t>(ifstream);
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
            ifstream.read(data, 127);
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

bool RP::getFileDataFromMemory(const char** dataPtr, std::uint64_t& size, const char* packfileData, std::uint64_t packfileSize, const char* filename)
{
    if(!checkIfPackfileFromMemory(packfileData, packfileSize))
    {
        return false;
    }

    std::uint64_t index = 5;

    std::uint16_t items = *((std::uint16_t*)(packfileData + index));
    index += 2;

    // get file location
    std::uint64_t location;
    std::uint64_t nextLocation;
    bool found = false;
    bool nextExists = false;
    for(std::uint16_t i = 0; i < items; ++i)
    {
        location = *((std::uint64_t*)(packfileData + index));
        index += 8;

        if(std::strcmp(packfileData + index, filename) == 0)
        {
            found = true;
            if(i + 1 < items)
            {
                nextLocation = *((std::uint64_t*)(packfileData + index + 127));
                nextExists = true;
            }
            break;
        }
        index += 127;
    }

    if(!found)
    {
        return false;
    }

    if(nextExists)
    {
        size = nextLocation - location;
    }
    else
    {
        size = packfileSize - location;
    }

    *dataPtr = packfileData + location;

    return true;
}

bool RP::getFileDataFromMemory(const char** dataPtr, std::uint64_t& size, const char* packfileData, std::uint64_t packfileSize, std::string filename)
{
    return getFileDataFromMemory(dataPtr, size, packfileData, packfileSize, filename.c_str());
}

std::string RP::getNameFromPath(std::string path, char delimeter)
{
    while(path.substr(path.size() - 1, 1).c_str()[0] == delimeter)
    {
        path = path.substr(0, path.size() - 1);
    }

    int pos = path.rfind(delimeter);

    return path.substr(pos + 1, path.size() - pos - 1);
}
