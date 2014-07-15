
#ifndef RESOURCE_PACKER_HPP
#define RESOURCE_PACKER_HPP

#define VALID_FILENAME_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_0123456789"
#define RW_BYTE_RATE 512

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__WIN32__)
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

#include <string>
#include <vector>
#include <list>
#include <fstream>

#ifndef NDEBUG
#include <iostream>
#endif

namespace RP
{
    struct PackInfo
    {
        unsigned short items;
        std::vector<std::string> names;
        std::vector<unsigned long long> locations;
    };

    bool checkIfFile(const char* name);
    bool checkIfFileExists(const char* name);
    bool checkIfValidFilename(std::string name);
    bool createPackfile(std::list<std::string> files, std::string packfileName);
    bool readPackfileInfo(std::string packfileName, PackInfo& packInfo);
    std::string getNameFromPath(std::string path);
}

#endif
