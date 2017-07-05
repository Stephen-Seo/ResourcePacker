
#ifndef RESOURCE_PACKER_HPP
#define RESOURCE_PACKER_HPP

#define RESOURCE_PACKER_IDENTIFIER "RPACK"
#define VALID_FILENAME_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_0123456789."
#define RW_BYTE_RATE 1024

#if defined(_WIN32)
  #include <windows.h>
  #if defined(BUILD_SHARED_LIBS)
    #if defined(ResourcePacker_EXPORTS)
      #define RP_EXPORT __declspec(dllexport)
    #else
      #define RP_EXPORT __declspec(dllimport)
    #endif
  #endif
#elif defined(__WIN32__)
  #include <windows.h>
  #if defined(BUILD_SHARED_LIBS)
    #if defined(ResourcePacker_EXPORTS)
      #define RP_EXPORT __declspec(dllexport)
    #else
      #define RP_EXPORT __declspec(dllimport)
    #endif
  #endif
#else
  #include <sys/stat.h>
#endif

#ifndef RP_EXPORT
  #define RP_EXPORT
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
    struct RP_EXPORT PackInfo
    {
        unsigned short items;
        std::vector<std::string> names;
        std::vector<unsigned long long> locations;
    };

    // checks if 'name' is an existing file and not a directory
    bool RP_EXPORT checkIfFile(const char* name);
    // checks if 'name' exists
    bool RP_EXPORT checkIfFileExists(const char* name);
    // checks if 'name' does not contain a valid character defined by VALID_FILENAME_CHARS
    bool RP_EXPORT checkIfValidFilename(std::string name);
    // checks if 'name' is a packfile (checks header, not contents)
    bool RP_EXPORT checkIfPackfile(const char* name);
    // checks if data is a packfile (checks header, not contents)
    bool RP_EXPORT checkIfPackfileFromMemory(const char* data, unsigned long long size);
    // creates a packfile with filenames in list 'files' and with name 'packfileName'
    bool RP_EXPORT createPackfile(std::list<std::string> files, std::string packfileName, bool overwrite = false);
    // gets info from the header of a packfile named 'packfileName'
    bool RP_EXPORT readPackfileInfo(std::string packfileName, PackInfo& packInfo);
    // gets a file's data from a packfile named 'packfile' of name 'filename'
    bool RP_EXPORT getFileData(std::unique_ptr<char[]>& dataPtr, unsigned long long& size, std::string packfile, std::string filename);
    // gets a pointer to a file's data in the packfile data specified by 'filename'
    bool RP_EXPORT getFileDataFromMemory(const char** dataPtr, unsigned long long& size, const char* packfileData, unsigned long long packfileSize, const char* filename);
    bool RP_EXPORT getFileDataFromMemory(const char** dataPtr, unsigned long long& size, const char* packfileData, unsigned long long packfileSize, std::string filename);
    // returns the filename of a file defined by path 'path'
    std::string RP_EXPORT getNameFromPath(std::string path);
}

#endif
