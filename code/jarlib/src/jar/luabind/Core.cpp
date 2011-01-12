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

#include "jar/luabind/Core.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/FileSystem.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/LuaLogger.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/Core.hpp"
#include "jar/core/CLArguments.hpp"
#include <physfs.h> //for PHYSFS_File
#include <unzip.h>
#include <string>

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp> //for pure_out_value
#include <luabind/iterator_policy.hpp>

namespace jar
{

//  Helpers for unz/MiniZip

static int GetCurrentUnzFileContent(unzFile filehandle, std::string& output)
{
    int status;
    if((status = unzOpenCurrentFile(filehandle)) != UNZ_OK)
    {
        return status;
    }
    char buf[1024];
    while((status = unzReadCurrentFile(filehandle, buf, sizeof(buf))) > 0)
    {
        //status returns number of bytes read
        output += std::string(buf, status);
    }
    unzCloseCurrentFile(filehandle);
    return status;
}

static unzFile MyUnzOpen(const std::string& filename)
{
    return unzOpen((CLArguments::GetSingleton().GetWorkingDirectory() + Core::GetSingleton().GetRootPath() + filename).c_str());
}

//  Helpers for the STL

/** luabind doesn't recognize the return value of std::vector<std::string>::at as a string, thus I need this helper function.
    That's all right since I need to change it to return nil on invalid indices anyway. **/
//note: I get an error if the return type is an std string* complaining about unregistered types. bullshit. anyway, using const char* instead works.
static const char * StringVectorAt(std::vector<std::string>& vec, unsigned int index)
{
    if(index >= vec.size()) return NULL;
    return vec[index].c_str();
}

void BindCore(lua_State* L)
{
    //for more convenient overloaded function matching
    typedef const bool (*Mount1Signature)(const std::string&, const bool);
    typedef const bool (*Mount2Signature)(const std::string&, const std::string&, const bool);
    typedef const bool (*ReadString1Signature)(PHYSFS_File*, std::string&);
    typedef const bool (*ReadString2Signature)(PHYSFS_File*, unsigned int, std::string&);

    //Didz suggested these functions may be used for evil and should be omitted in C++, instead of just overwritten in Lua.
    luabind::globals(L)["dofile"] = luabind::nil;
    luabind::globals(L)["loadfile"] = luabind::nil;
    luabind::globals(L)["package"]["loaders"] = luabind::nil;

    luabind::module(L)
    [
        // unz - MiniZip

        luabind::namespace_("unz")
        [
            luabind::class_<unzFile>("File")
                .def("Close", &unzClose)
                .def("LocateFile", &unzLocateFile)
                .def("GetCurrentFileContent", &GetCurrentUnzFileContent),
            luabind::def("Open", &MyUnzOpen)
        ],

        // std - C++ Standard Template Library

        luabind::namespace_("std")
        [
            luabind::class_<std::vector<std::string> >("stringvector")
                .def("at", &StringVectorAt)
                .def("__len", &std::vector<std::string>::size)
                .def("size", &std::vector<std::string>::size)
        ],

        // jar - Jedi Academy: Renaissance

        luabind::namespace_("jar")
        [
            luabind::class_<Logger, LuaLogger>("Logger")
                .def(luabind::constructor<>())
                .def("Log", &Logger::Log, &LuaLogger::WrapLog)
                .def("Warning", &Logger::Warning, &LuaLogger::WrapWarning)
                .def("Error", &Logger::Error, &LuaLogger::WrapError)
                .def("Info", &Logger::Info)
                .def("IncreaseLoggingLevel", &Logger::IncreaseLoggingLevel)
                .def("DecreaseLoggingLevel", &Logger::DecreaseLoggingLevel)
                .def("GetLoggingLevel", &Logger::GetLoggingLevel)
                .def("SetLoggingLevel", &Logger::SetLoggingLevel)
                .scope
                [
                    luabind::def("GetDefaultLogger", &Logger::GetDefaultLogger),
                    luabind::def("SetDefaultLogger", &Logger::SetDefaultLogger)
                ],

            luabind::def("GetFilesInDirectory", &Helpers::GetFilesInDirectory, luabind::return_stl_iterator),
            luabind::def("GetDirectoriesInDirectory", &Helpers::GetDirectoriesInDirectory, luabind::return_stl_iterator),

            luabind::class_<Core>("Core")
                .def("Update", &Core::Update)
                .scope
                [
                    luabind::def("GetSingleton", &Core::GetSingleton)
                ],

            // jar.fs - FileSystem

            luabind::namespace_("fs")
            [
                luabind::def("GetLastError", &fs::GetLastError),
                luabind::def("Mount", (Mount1Signature) &fs::Mount),
                luabind::def("Mount", (Mount2Signature) &fs::Mount),
                luabind::def("OpenRead", &fs::OpenRead),
                luabind::def("OpenWrite", &fs::OpenWrite),
                luabind::def("Unmount", &fs::Unmount),
                luabind::def("GetFilesInDirectory", &fs::GetFilesInDirectory, luabind::return_stl_iterator),
                luabind::def("GetDirectoriesInDirectory", &fs::GetDirectoriesInDirectory, luabind::return_stl_iterator),

                luabind::class_<PHYSFS_File>("File")
                    .def("Close", &fs::Close)
                    .def("ReadChar", &fs::ReadChar, luabind::pure_out_value(_2))
                    .def("ReadString", (ReadString1Signature) &fs::ReadString, luabind::pure_out_value(_2))
                    .def("ReadString", (ReadString2Signature) &fs::ReadString, luabind::pure_out_value(_3))
                    .def("ReadFloat", &fs::ReadFloat, luabind::pure_out_value(_2))
                    .def("ReadInt", &fs::ReadInt, luabind::pure_out_value(_2))
                    .def("ReadUnsignedInt", &fs::ReadUnsignedInt, luabind::pure_out_value(_2))
                    //2nd parameter is a pure out reference -> used as return value
                    //i.e. you do "local success, content = file:GetContent()"
                    .def("GetContent", &fs::GetCurrentFileContent, luabind::pure_out_value(_2))
                    .def("EOF", &fs::EndOfFile)
                    .def("Seek", &fs::Seek)
                    .def("Tell", &fs::Tell)
                    .def("WriteString", &fs::WriteString)
                    //TODO: WriteInt etc.
            ]
        ]
    ];

    luabind::globals(L)["unz"]["OK"] = UNZ_OK;
    luabind::globals(L)["unz"]["END_OF_LIST_OF_FILE"] = UNZ_END_OF_LIST_OF_FILE;
    luabind::globals(L)["unz"]["ERRNO"] = UNZ_ERRNO;
    luabind::globals(L)["unz"]["EOF"] = UNZ_EOF;
    luabind::globals(L)["unz"]["PARAMERROR"] = UNZ_PARAMERROR;
    luabind::globals(L)["unz"]["BADZIPFILE"] = UNZ_BADZIPFILE;
    luabind::globals(L)["unz"]["INTERNALERROR"] = UNZ_INTERNALERROR;
    luabind::globals(L)["unz"]["CRCERROR"] = UNZ_CRCERROR;
}

}
