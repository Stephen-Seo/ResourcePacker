
#ifndef RESOURCE_PACKER_HPP
#define RESOURCE_PACKER_HPP

#define RESOURCE_PACKER_IDENTIFIER "RPACK"
#define VALID_FILENAME_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_0123456789."
#define RW_BYTE_RATE 512

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__WIN32__)
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

#include <cstring>
#include <string>
#include <locale>
#include <vector>
#include <list>
#include <memory>
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

    // checks if 'name' is an existing file and not a directory
    bool checkIfFile(const char* name);
    // checks if 'name' exists
    bool checkIfFileExists(const char* name);
    // checks if 'name' does not contain a valid character defined by VALID_FILENAME_CHARS
    bool checkIfValidFilename(std::string name);
    // checks if 'name' is a packfile (checks header, not contents)
    bool checkIfPackfile(const char* name);
    // creates a packfile with filenames in list 'files' and with name 'packfileName'
    bool createPackfile(std::list<std::string> files, std::string packfileName);
    // gets info from the header of a packfile named 'packfileName'
    bool readPackfileInfo(std::string packfileName, PackInfo& packInfo);
    // gets a file's data from a packfile named 'packfile' of name 'filename'
    bool getFileData(std::unique_ptr<char[]>& dataPtr, unsigned long long& size, std::string packfile, std::string filename);
    // returns the filename of a file defined by path 'path'
    std::string getNameFromPath(std::string path);
}

#endif
