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

#include "jar/luabind/FileSystem.hpp"

#include "jar/core/Logger.hpp"
#include "jar/core/FileSystem.hpp"
#include "jar/core/CLArguments.hpp"
#include "jar/Core.hpp"

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/iterator_policy.hpp>

#include <physfs.h> //for PHYSFS_File
#include <unzip.h>
#include <string>

namespace jar
{

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

//currently not used, subject to deletion
void BindFileSystem(lua_State* L)
{
    //minizip
    luabind::module(L, "unz")
    [
        luabind::class_<unzFile>("File")
            .def("Close", &unzClose)
            .def("LocateFile", &unzLocateFile)
            .def("GetCurrentFileContent", &GetCurrentUnzFileContent),
        luabind::def("Open", &MyUnzOpen)
    ];
    luabind::globals(L)["unz"]["OK"] = UNZ_OK;
    luabind::globals(L)["unz"]["END_OF_LIST_OF_FILE"] = UNZ_END_OF_LIST_OF_FILE;
    luabind::globals(L)["unz"]["ERRNO"] = UNZ_ERRNO;
    luabind::globals(L)["unz"]["EOF"] = UNZ_EOF;
    luabind::globals(L)["unz"]["PARAMERROR"] = UNZ_PARAMERROR;
    luabind::globals(L)["unz"]["BADZIPFILE"] = UNZ_BADZIPFILE;
    luabind::globals(L)["unz"]["INTERNALERROR"] = UNZ_INTERNALERROR;
    luabind::globals(L)["unz"]["CRCERROR"] = UNZ_CRCERROR;

    typedef const bool (*Mount1Signature)(const std::string&, const bool);
    typedef const bool (*Mount2Signature)(const std::string&, const std::string&, const bool);
    typedef const bool (*ReadString1Signature)(PHYSFS_File*, std::string&);
    typedef const bool (*ReadString2Signature)(PHYSFS_File*, unsigned int, std::string&);
    //filesystem
    luabind::module(L, "jar")
    [
    luabind::namespace_("fs")
    [
        luabind::def("GetLastError", &fs::GetLastError),
        luabind::def("Mount", (Mount1Signature) &fs::Mount),
        luabind::def("Mount", (Mount2Signature) &fs::Mount),
        luabind::def("OpenRead", &fs::OpenRead),
        luabind::def("OpenWrite", &fs::OpenWrite),
        luabind::def("Unmount", &fs::Unmount),
        luabind::def("GetFilesInDirectory", &fs::GetFilesInDirectory, luabind::return_stl_iterator),
        luabind::def("GetDirectoriesInDirectory", &fs::GetDirectoriesInDirectory, luabind::return_stl_iterator)
    ]
    ];

    luabind::module(L, "jar")
    [
    luabind::namespace_("fs")
    [
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
    ];
}

}
