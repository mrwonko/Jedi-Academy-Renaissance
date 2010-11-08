/*
===========================================================================
Copyright (C) 2010 Willi Schinmeyer

This file is part of the Jedi Academy: Renaissance source code.

Jedi Academy: Renaissance source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Jedi Academy: Renaissance source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jedi Academy: Renaissance source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

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
