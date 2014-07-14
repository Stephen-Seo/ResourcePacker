
#ifndef RESOURCE_PACKER_HPP
#define RESOURCE_PACKER_HPP

#define VALID_FILENAME_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_0123456789"

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

namespace RP
{
    struct PackInfo
    {
        unsigned int items;
        std::vector<std::string> names;
        std::vector<unsigned int> locations;
    };

    bool checkIfFile(const char* name);
    bool checkIfFileExists(const char* name);
    bool checkIfValidFilename(std::string name);
    bool createPackfile(std::list<std::string> files, std::string packfileName);
    bool readPackfile(std::string packfileName, PackInfo& packInfo);
    std::string getNameFromPath(std::string path);
}

#endif
