
#include <iostream>

#include <cstring>
#include <string>
#include <list>

#include "ResourcePacker.hpp"

enum Type
{
    READ_TOC,
    WRITE
};

void printHelp()
{
    std::cout << "  Usage:\n";
    std::cout << "Specify the name of the packfile to get info about the file.\n";
    std::cout << "Option '-w' will allow you to create a packfile instead.\n";
    std::cout << "    You will be prompted to set the packfile name.\n";
    std::cout << "Option '-h' will print this notice.\n";
    std::cout << "Option '--testGet' will retrieve a file from the packfile but do nothing with it.\n";
    std::cout << "    example: '--testGet packfileName filename'\n";
}

int main(int argc, char** argv)
{
    Type type = READ_TOC;

    // Skip program name invocation string
    --argc;
    ++argv;

    if(argc == 0)
    {
        printHelp();
        return 0;
    }

    std::list<std::string> inputList;

    // Get inputs
    while(argc > 0)
    {
        if(std::strcmp(argv[0],"-w") == 0)
        {
            type = WRITE;
            --argc;
            ++argv;
            continue;
        }
        else if(std::strcmp(argv[0],"-h") == 0)
        {
            printHelp();
            return 0;
        }
        else if(std::strcmp(argv[0],"--testGet") == 0)
        {
            --argc;
            ++argv;
            if(argc != 2)
            {
                std::cout << "Invalid number of arguments for option '--testGet'.\n";
                return -1;
            }

            std::string packfileName(argv[0]), filename(argv[1]);
            std::unique_ptr<char[]> data;
            unsigned long long size;

            if(!RP::getFileData(data, size, packfileName, filename))
            {
                std::cout << "Failed to retrieve file from packfile.\n";
                return -1;
            }

            std::cout << "Retrieved " << filename << " of size " << size << '\n';
            return 0;
        }

#ifndef NDEBUG
        std::cout << argc << ": " << argv[0] << '\n';
#endif

        if(RP::checkIfFile(argv[0]))
        {
#ifndef NDEBUG
            std::cout << argv[0] << " is file\n";
#endif
            inputList.push_back(std::string(argv[0]));
        }

        --argc;
        ++argv;
    }

    if(inputList.size() == 1 && type == READ_TOC)
    {
    // Read table of contents of pack file
        RP::PackInfo packInfo;
        if(!RP::readPackfileInfo(inputList.front(), packInfo))
        {
            std::cout << "Failed to read packfile. Packfile is corrupted or not a packfile.\n";
            return -1;
        }

        std::cout << "\n";
        std::cout << packInfo.items << " items found in pack. They are as follows:\n";
        for(auto iter = packInfo.names.begin(); iter != packInfo.names.end(); ++iter)
        {
            std::cout << "  " << *iter << '\n';
        }
    }
    else if(inputList.size() >= 1 && type == WRITE)
    {
    // Create new pack file with specified files
        std::string outfile = "";
        std::cout << "Please specify a name for the new packfile.\n";
        std::cin >> outfile;

        if(RP::checkIfValidFilename(outfile))
        {
#ifndef NDEBUG
            std::cout << "Input filename \"" << outfile << "\" is valid.\n";
#endif
            std::cout << "Creating packfile with name " << outfile << '\n';
            if(RP::createPackfile(inputList, outfile))
                return 0;
            else
                return -1;
        }
        else
        {
            std::cout << "Input filename \"" << outfile << "\" is invalid.\n";
            return -1;
        }
    }

    return 0;
}
