
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
    std::cout << "    Usage:\n";
    std::cout << "Specify the name of the packfile to get info about the file.\n";
    std::cout << "Option '-w' will allow you to create a packfile instead.\n";
    std::cout << "\tPlease specify filenames to place in the packfile when writing.\n";
    std::cout << "Option '-h' will print this notice.\n";
}

int main(int argc, char** argv)
{
    Type type = READ_TOC;

    // Skip program name invocation string
    --argc;
    ++argv;

    std::list<std::string> inputList;

    // Get inputs
    while(argc > 0)
    {
        if(strcmp(argv[0],"-w") == 0)
        {
            type = WRITE;
            --argc;
            ++argv;
            continue;
        }
        else if(strcmp(argv[0],"-h") == 0)
        {
            printHelp();
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
            if(RP::createPackfile(inputList, outfile))
                return 0;
            else
                return -1;
        }
        else
        {
#ifndef NDEBUG
            std::cout << "Input filename \"" << outfile << "\" is invalid.\n";
#endif
            return -1;
        }
    }

    return 0;
}
