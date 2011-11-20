#include "Parser.h"
#include "Config.h"

#include <iostream>

Parser::Parser(const Config& config) :
    mConfig(config),
    mFilesystem(config.GetIncludeDirs())
{
    //ctor

    //todo: remove test
    std::string input;
    while(true)
    {
        std::cout<<"Enter a filename to look up or exit to exit."<<std::endl;
        std::getline(std::cin, input);
        if(input == "exit") break;
        std::string filename = mFilesystem.FindFile(input);
        if(filename == "")
        {
            std::cout<<"404 File not found!"<<std::endl;
        }
        else
        {
            std::cout<<filename<<std::endl;
        }
    }
}

Parser::~Parser()
{
    //dtor
}
