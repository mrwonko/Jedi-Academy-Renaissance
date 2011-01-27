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

#include "jar/core/Helpers.hpp"
#include "jar/Core.hpp"
#include "jar/core/CLArguments.hpp"
#include "jar/core/Logger.hpp"
#include <string>
#include <cctype>
#include <algorithm>
#include <cctype>

//TODO: delete
#include <iostream>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace jar
{
    // removew whitespaces from beginning and end
    std::string Helpers::Trim(const std::string& string)
    {
        //this is the trimmed version of the string
        std::string trimmedString = string;

        //now remove the whitespaces from the beginning
        trimmedString.erase(0, trimmedString.find_first_not_of(" \t"));

        //and remove the whitespaces from the end
        trimmedString.erase(trimmedString.find_last_not_of(" \t")+1);

        return trimmedString;
    }

    std::string Helpers::IntToString(int integer)
    {
        std::stringstream ss;
        ss<<integer;
        return ss.str();
    }

    std::string Helpers::FloatToString(float number)
    {
        std::stringstream ss;
        ss<<number;
        return ss.str();
    }

    int Helpers::StringToInt(const std::string& string)
    {
        std::stringstream ss;
        ss<<string;
        int result;
        ss>>result;
        return result;
    }

    float Helpers::StringToFloat(const std::string& string)
    {
        std::stringstream ss;
        ss<<string;
        float result;
        ss>>result;
        return result;
    }

    std::string Helpers::RemoveCStyleComments(const std::string& textWithComments)
    {
        std::string textWithoutComments = textWithComments;

        std::string::size_type commentStart, commentEnd, lastCommand = 0;

        //remove multi line comments first because else a // could hide a following */
        //but keep in mind that a /* after a // doesn't count.

        //while you can find mutli-line-comment-starts
        while( (commentStart = textWithoutComments.find("/*", lastCommand)) != std::string::npos)
        {
            commentStart += lastCommand;
            //see if the beginning of the comment is within a // comment on the same line
            std::string::size_type lastLinebreak = textWithoutComments.substr(0, commentStart).find_last_of("\n");
            lastLinebreak += lastCommand;
            if(lastLinebreak != std::string::npos && textWithoutComments.find("//", lastLinebreak) + lastLinebreak < commentStart)
            {
                //if that's the case: ignore this.
                lastCommand = commentStart;
                continue;
            }
            //search for the end of the commend - if you can't find any, stop searching for any other multi-line-comments.
            if( (commentEnd = textWithoutComments.find("*/", commentStart)) == std::string::npos) break;

            //remove the found multi-line-comment
            textWithoutComments.erase(commentStart, commentEnd - commentStart + 2);
            lastCommand = commentStart;
        }

        //remove one line comments
        while( (commentStart = textWithoutComments.find("//")) != std::string::npos)
        {
            commentEnd = textWithoutComments.find("\n", commentStart);
            if(commentEnd == std::string::npos)
            {
                textWithoutComments.erase(commentStart);
            }
            else
            {
                textWithoutComments.erase(commentStart, commentEnd - commentStart);
            }
        }

        return textWithoutComments;
    }

    std::string& Helpers::ToLower(std::string& text)
    {
        std::transform(text.begin(), text.end(), text.begin(), (int (*)(int))std::tolower);
        return text;
    }

    std::string& Helpers::ToUpper(std::string& text)
    {
        std::transform(text.begin(), text.end(), text.begin(), (int (*)(int))std::toupper);
        return text;
    }

    std::string Helpers::Replace(const std::string& text, const std::string& pattern, const std::string& replace)
    {
        //if the replace string contains the pattern do nothing as it would only result in an endless loop
        if(replace.find(pattern) != std::string::npos)
        {
            return text;
        }

        std::string result = text;
        std::string::size_type match = result.find(pattern);


        //as long as the string still contains the search pattern
        while( match != std::string::npos)
        {
            //replace it
            result.erase(match, pattern.length());
            result.insert(match, replace);

            //search next
            match = result.find(pattern);
        }
        return result;
    }

    int Helpers::Round(float f)
    {
        return((int)(f+0.5));
    }

#if defined(_WIN32)
    //Since the functions for checking files and directories are mostly the same I use function pointers for the part that's different.
    static const bool IsFile(const WIN32_FIND_DATA& ffd)
    {
        return(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
    }

    static const bool IsDirectory(const WIN32_FIND_DATA& ffd)
    {
        if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) return false;
        std::string name = std::string(ffd.cFileName);
        if(name.length() == 0 or name[0] == '.') return false;
        return true;
    }

    typedef const bool (*CheckStuffCorrectFunction) (const WIN32_FIND_DATA&);

    //see http://msdn.microsoft.com/en-us/library/aa365200%28VS.85%29.aspx
    static std::vector<std::string> GetStuffInDirectory(const std::string& directoryPath, CheckStuffCorrectFunction checkFunc)
    {
        std::vector<std::string> filenames;
        if(directoryPath.length() + 3 > MAX_PATH)
        {
            Logger::GetDefaultLogger().Error("Helpers::GetStuffInDirectory(): directoryPath to long!");
            return filenames;
        }
        std::string path = directoryPath + "\\*";
        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFile(path.c_str(), &ffd);
        if(hFind == INVALID_HANDLE_VALUE)
        {
            Logger::GetDefaultLogger().Warning("Helpers::GetStuffInDirectory(): invalid directory!");
            return filenames;
        }
        do
        {
            if(checkFunc(ffd))
            {
                filenames.push_back(std::string(ffd.cFileName));
            }
        }
        while(FindNextFile(hFind, &ffd) != 0);
        if(GetLastError() != ERROR_NO_MORE_FILES)
        {
            Logger::GetDefaultLogger().Warning("Helpers::GetStuffInDirectory(): Could not list all files, error code " + Helpers::IntToString(GetLastError()));
        }
        FindClose(hFind);
        return filenames;
    }
#endif

    namespace
    {
        std::vector<std::string> g_tempVec;
    }

    std::vector<std::string>& Helpers::GetFilesInDirectory(std::string directoryPath)
    {
        directoryPath = CLArguments::GetSingleton().GetWorkingDirectory() + Core::GetSingleton().GetRootPath() + directoryPath;
        g_tempVec.clear();
#if defined(_WIN32)
        g_tempVec = GetStuffInDirectory(directoryPath, &IsFile);
#else
        Logger::GetDefaultLogger().Error("Helpers::GetFilesInDirectory() not implemented for current platform!");
#warning Helpers::GetFilesInDirectory() not implemented for this platform
#endif
        return g_tempVec;
    }

    std::vector<std::string>& Helpers::GetDirectoriesInDirectory(std::string directoryPath)
    {
        directoryPath = CLArguments::GetSingleton().GetWorkingDirectory() + Core::GetSingleton().GetRootPath() + directoryPath;
        g_tempVec.clear();
#if defined(_WIN32)
        g_tempVec = GetStuffInDirectory(directoryPath, &IsDirectory);
#else
        Logger::GetDefaultLogger().Error("Helpers::GetDirectoriesInDirectory() not implemented for current platform!");
#warning Helpers::GetFilesInDirectory() not implemented for this platform
#endif
        return g_tempVec;
    }

    const bool Helpers::CaseInsensitiveStringLessThan(std::string str1, std::string str2)
    {
        return(Helpers::ToLower(str1) < Helpers::ToLower(str2));
    }
}

