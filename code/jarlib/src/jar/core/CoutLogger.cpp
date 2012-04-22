#include "jar/core/CoutLogger.hpp"
#include <iostream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace jar
{

    CoutLogger::CoutLogger()
    {
        //ctor
    }

    CoutLogger::~CoutLogger()
    {
        //dtor
    }

    void CoutLogger::Log(const std::string& message)
    {
        std::cout<<message<<std::endl;
    }

    void CoutLogger::Warning(const std::string& message)
    {
#ifdef _WIN32
        HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );
        WORD wOldColorAttrs;
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        //save current color
        GetConsoleScreenBufferInfo(h, &csbiInfo);
        wOldColorAttrs = csbiInfo.wAttributes;
        //Set new color
        SetConsoleTextAttribute ( h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY );
#endif
        std::cout<<"Warning: "<<message<<std::endl;
#ifdef _WIN32
        //reset color
        SetConsoleTextAttribute ( h, wOldColorAttrs);
#endif
    }

    void CoutLogger::Error(const std::string& message)
    {
#ifdef _WIN32
        HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );
        WORD wOldColorAttrs;
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        //save current color
        GetConsoleScreenBufferInfo(h, &csbiInfo);
        wOldColorAttrs = csbiInfo.wAttributes;
        //Set new color
        SetConsoleTextAttribute ( h, FOREGROUND_RED | FOREGROUND_INTENSITY );
#endif
        std::cout<<"Error: "<<message<<std::endl;
#ifdef _WIN32
        //reset color
        SetConsoleTextAttribute ( h, wOldColorAttrs);
#endif
    }

} // namespace jar
