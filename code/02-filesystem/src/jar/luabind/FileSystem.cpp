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
#include "jar/core/CLArguments.hpp"
#include "jar/Globals.hpp"

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
    return unzOpen((jar::CLArguments::GetSingleton().GetWorkingDirectory() + Globals::rootDir + filename).c_str());
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
    //physfs
    luabind::module(L, "physFS")
    [
        luabind::def("GetLastError", &PHYSFS_getLastError),
        luabind::def("Mount", (Mount1Signature) &PhysFSMount),
        luabind::def("Mount", (Mount2Signature) &PhysFSMount),
        luabind::def("Unmount", &PhysFSUnmount),
        luabind::def("OpenRead", &PHYSFS_openRead),
        luabind::def("OpenWrite", &PHYSFS_openWrite),
        luabind::class_<PHYSFS_File>("File")
            .def("Close", &PHYSFS_close)
            //2nd parameter is a pure out reference -> used as return value
            //i.e. you do "local success, content = file:GetContent()"
            .def("GetContent", &PhysFSGetCurrentFileContent, luabind::pure_out_value(_2))
            .def("ReadChar", &PhysFSReadChar, luabind::pure_out_value(_2))
            .def("ReadString", (ReadString1Signature) &PhysFSReadString, luabind::pure_out_value(_2))
            .def("ReadString", (ReadString2Signature) &PhysFSReadString, luabind::pure_out_value(_3))
            .def("ReadFloat", &PhysFSReadFloat, luabind::pure_out_value(_2))
            .def("ReadInt", &PhysFSReadInt, luabind::pure_out_value(_2))
            .def("ReadUnsignedInt", &PhysFSReadUnsignedInt, luabind::pure_out_value(_2))
            .def("EOF", &PhysFSEOF)
            .def("Seek", &PhysFSSeek)
            .def("Tell", &PHYSFS_tell)
            .def("WriteString", &PhysFSWriteString)
            //TODO: WriteInt etc.
    ];
}

}
