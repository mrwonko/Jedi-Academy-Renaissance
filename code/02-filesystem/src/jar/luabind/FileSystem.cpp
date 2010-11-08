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
#include "jar/core/PhysFSFile.hpp"

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp>

#include <unzip.h>
#include <physfs.h>
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
    return unzOpen((jar::MODDIR + filename).c_str());;
}

void BindFileSystem(lua_State* L)
{
    //TODO: complete Lua documentation
    //minizip
    luabind::module(L, "unz")
    [
        luabind::class_<unzFile>("File")
            .def("Close", &unzClose)
            .def("LocateFile", &unzLocateFile)
            .def("GetCurrentFileContent", &GetCurrentUnzFileContent),
        luabind::def("Open", &MyUnzOpen)
    ];
    typedef const int (*Mount1Signature)(const std::string&, const bool);
    typedef const int (*Mount2Signature)(const std::string&, const std::string&, const bool);
    typedef std::string (*ReadString1Signature)(PHYSFS_File*);
    typedef std::string (*ReadString2Signature)(PHYSFS_File*, unsigned int);
    //physfs
    luabind::module(L, "physFS")
    [
        luabind::def("GetLastError", &PHYSFS_getLastError),
        luabind::def("Mount", (Mount1Signature) &PhysFSMount), //TODO: document different syntax!
        luabind::def("Mount", (Mount2Signature) &PhysFSMount),
        luabind::def("Unmount", &PHYSFS_removeFromSearchPath),
        luabind::def("OpenRead", &PHYSFS_openRead),
        luabind::def("OpenWrite", &PHYSFS_openWrite),
        luabind::class_<PHYSFS_File>("File")
            .def("Close", &PHYSFS_close)
            //2nd parameter is a pure out reference -> used as return value
            //i.e. you do "local success, content = file:GetContent()"
            .def("GetContent", &PhysFSGetCurrentFileContent, luabind::pure_out_value(_2))
            .def("ReadChar", &PhysFSReadChar)
            .def("ReadString", (ReadString1Signature) &PhysFSReadString)
            .def("ReadString", (ReadString2Signature) &PhysFSReadString)
            .def("ReadFloat", &PhysFSReadFloat)
            .def("ReadInt", &PhysFSReadInt)
            .def("ReadUnsignedInt", &PhysFSReadUnsignedInt)
            .def("EOF", &PhysFSEOF)
            .def("Seek", &PHYSFS_seek)
            .def("Tell", &PHYSFS_tell)
            .def("WriteString", &PhysFSWriteString)
    ];
}

}
