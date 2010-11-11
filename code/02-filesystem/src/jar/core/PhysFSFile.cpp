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

#include "jar/core/PhysFSFile.hpp"
#include "jar/Globals.hpp"
#include "jar/core/CLArguments.hpp"
#include "jar/core/Logger.hpp"
#include <physfs.h>

namespace jar
{

const bool PhysFSGetCurrentFileContent(PHYSFS_File* file, std::string& output)
{
    char buf[1024];
    int status;
    //PHYSFS_read returns the read characters.
    while((status = PHYSFS_read(file, &buf, sizeof(char), sizeof(buf))) > 0)
    {
        output += std::string(buf, status);
    }
    //-1 on error
    if(status == -1)
    {
#ifdef _DEBUG
        Logger::GetDefaultLogger().Warning(PHYSFS_getLastError());
#endif
        return false;
    }
    //if no error was set, the reason for status == 0 should be EOF
    if(!PHYSFS_eof(file))
    {
#ifdef _DEBUG
        Logger::GetDefaultLogger().Warning("PHYSFS_read(): Nothing read, but eof not reached and error not set, either.");
#else
        Logger::GetDefaultLogger().Info("PHYSFS_read(): Nothing read, but eof not reached and error not set, either.", 1);
#endif
    }
    return true;
}

const bool PhysFSMount(const std::string& filename, const bool append)
{
    return PHYSFS_mount((CLArguments::GetSingleton().GetWorkingDirectory() + Globals::rootDir + filename).c_str(), NULL, append);
}

const bool PhysFSMount(const std::string& filename, const std::string& mountpoint, const bool append)
{
    return PHYSFS_mount((CLArguments::GetSingleton().GetWorkingDirectory() + Globals::rootDir + filename).c_str(), mountpoint.c_str(), append);
}

const bool PhysFSUnmount(const std::string& filename)
{
    return PHYSFS_removeFromSearchPath((CLArguments::GetSingleton().GetWorkingDirectory() + Globals::rootDir + filename).c_str());
}

const bool PhysFSReadInt(PHYSFS_File* file, int& out_num)
{
    return PHYSFS_readSLE32(file, &out_num);
}

const bool PhysFSReadUnsignedInt(PHYSFS_File* file, unsigned int& out_num)
{
    return PHYSFS_readULE32(file, &out_num);
}

const bool PhysFSReadFloat(PHYSFS_File* file, float& out_num)
{
    return PHYSFS_readSLE32(file, (PHYSFS_sint32*)&out_num); //TODO: check this works.
}

const bool PhysFSReadChar(PHYSFS_File* file, char& output)
{
    return (PHYSFS_read(file, &output, 1, 1) == 1);
}

const bool PhysFSReadString(PHYSFS_File* file, std::string& output)
{
    if(PHYSFS_eof(file))
    {
        Logger::GetDefaultLogger().Warning("PhysFS.File.ReadString(): already at eof!");
        return true;
    }
    while(true)
    {
        char c;
        if(PHYSFS_read(file, &c, 1, 1) != 1)
        {
            if(PHYSFS_eof(file))
            {
                //eof? that's ok.
                break;
            }
            //not eof? then something went wrong.
            return false;
        }
        output += c;
        if(c == '\0')
        {
            break;
        }
    }
    return true;
}

const bool PhysFSReadString(PHYSFS_File* file, unsigned int len, std::string& output)
{
    if(PHYSFS_eof(file))
    {
        Logger::GetDefaultLogger().Warning("PhysFS.File.ReadString(): already at eof!");
        return true;
    }
    for(unsigned int i = 0; i < len; ++i)
    {
        char c;
        if(PHYSFS_read(file, &c, 1, 1) != 1)
        {
            if(PHYSFS_eof(file))
            {
                //eof? that's ok.
                break;
            }
            //not eof? then something went wrong.
            return false;
        }
        output += c;
    }
    return true;
}

const bool PhysFSEOF(PHYSFS_File* file)
{
    return PHYSFS_eof(file);
}

const bool PhysFSWriteString(PHYSFS_File* file, const std::string& str)
{
    return(PHYSFS_write(file, str.c_str(), 1, str.length()) == str.length());
}

const bool PhysFSSeek(PHYSFS_File* file, const int64_t position) //TODO: test whether luabind treats int64_t as number
{
    return PHYSFS_seek(file, position);
}

}
