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

#include "jar/core/FileSystem.hpp"
#include "jar/Core.hpp"
#include "jar/core/CLArguments.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"
#include <physfs.h>
#include <set>

namespace jar
{
namespace fs
{

const bool Init(char* argv0)
{
    return PHYSFS_init(argv0);
}

const bool Deinit()
{
    return PHYSFS_deinit();
}

const bool SetWriteDir(const std::string& writedir)
{
    return PHYSFS_setWriteDir((CLArguments::GetSingleton().GetWorkingDirectory() + Core::GetSingleton().GetRootPath() + writedir).c_str());
}

const std::string GetLastError()
{
    const char* err = PHYSFS_getLastError();
    return err == NULL ? "" : err;
}

PHYSFS_File* OpenRead(const std::string& filename)
{
    return PHYSFS_openRead(filename.c_str());
}

PHYSFS_File* OpenWrite(const std::string& filename)
{
    return PHYSFS_openWrite(filename.c_str());
}

const bool Close(PHYSFS_File* file)
{
    return PHYSFS_close(file);
}

const bool ReadFile(const std::string& filename, std::string& output)
{
    PHYSFS_File* file = OpenRead(filename);
    if(not file)
    {
        return false;
    }
    if(not GetCurrentFileContent(file, output))
    {
        assert(Close(file));
        return false;
    }
    assert(Close(file));
    return true;
}

const bool GetCurrentFileContent(PHYSFS_File* file, std::string& output)
{
    if(PHYSFS_fileLength(file) == 0) return true;

    char buf[1024];
    int status;
    //PHYSFS_read returns the read characters.
    while(true)
    {
        status = PHYSFS_read(file, &buf, sizeof(char), sizeof(buf));
        output += std::string(buf, status);
        if(status < (int)sizeof(buf)) break; //eof reached
    }
    //-1 on error
    if(status == -1)
    {
        //it's up to the user to get the error, if we do it here, the user will not be able to do it himself.
        return false;
    }
    //if no error was set, the reason for status == 0 should be EOF
    if(!PHYSFS_eof(file))
    {
#ifdef _DEBUG
        Logger::GetDefaultLogger().Warning(std::string("PHYSFS_read(): Nothing read, but eof not reached and error not set, either; PhysFS says: ")+PHYSFS_getLastError());
#else
        Logger::GetDefaultLogger().Info(std::string("PHYSFS_read(): Nothing read, but eof not reached and error not set, either; PhysFS says: ")+PHYSFS_getLastError(), 1);
#endif
    }
    return true;
}

const bool Mount(const std::string& filename, const bool append)
{
    return PHYSFS_mount((CLArguments::GetSingleton().GetWorkingDirectory() + Core::GetSingleton().GetRootPath() + filename).c_str(), NULL, append);
}

const bool Mount(const std::string& filename, const std::string& mountpoint, const bool append)
{
    return PHYSFS_mount((CLArguments::GetSingleton().GetWorkingDirectory() + Core::GetSingleton().GetRootPath() + filename).c_str(), mountpoint.c_str(), append);
}

const bool Unmount(const std::string& filename)
{
    return PHYSFS_removeFromSearchPath((CLArguments::GetSingleton().GetWorkingDirectory() + Core::GetSingleton().GetRootPath() + filename).c_str());
}

const bool ReadInt(PHYSFS_File* file, int& out_num)
{
    return PHYSFS_readSLE32(file, &out_num);
}

const bool ReadUnsignedInt(PHYSFS_File* file, unsigned int& out_num)
{
    return PHYSFS_readULE32(file, &out_num);
}

const bool ReadFloat(PHYSFS_File* file, float& out_num)
{
    return PHYSFS_readSLE32(file, (PHYSFS_sint32*)&out_num); //TODO: check this works.
}

const bool ReadChar(PHYSFS_File* file, char& output)
{
    return (PHYSFS_read(file, &output, 1, 1) == 1);
}

const bool ReadString(PHYSFS_File* file, std::string& output)
{
    if(PHYSFS_eof(file))
    {
        Logger::GetDefaultLogger().Warning(".File.ReadString(): already at eof!");
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

const bool ReadString(PHYSFS_File* file, unsigned int len, std::string& output)
{
    if(PHYSFS_eof(file))
    {
        Logger::GetDefaultLogger().Warning(".File.ReadString(): already at eof!");
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

const bool EndOfFile(PHYSFS_File* file)
{
    return PHYSFS_eof(file);
}

const bool WriteString(PHYSFS_File* file, const std::string& str)
{
    return(PHYSFS_write(file, str.c_str(), 1, str.length()) == str.length());
}

const bool Seek(PHYSFS_File* file, const int64_t position) //TODO: test whether luabind treats int64_t as number
{
    return PHYSFS_seek(file, position);
}

const int64_t Tell(PHYSFS_File* file)
{
    return PHYSFS_tell(file);
}

namespace
{
    struct StringSetComparator //Case insensitive
    {
        bool operator()(const std::string& lhs, const std::string& rhs) const
        {
            return Helpers::CaseInsensitiveStringLessThan(lhs, rhs);
        }
    };

    typedef std::set<std::string, StringSetComparator> caseInsStringSet;

    //callback for file/directory enumeration
    //onlyFiles = true -> we want only files
    //onlyFiles = false -> we want only directories!
    static void AddToSet(caseInsStringSet& set, std::string dir, std::string filename, const bool onlyFiles)
    {
        //append a / to the directory if it isn't already
        if(dir.length() && dir[dir.length()-1] != '/')
        {
            dir += '/';
        }

        //is this a file/directory and do we want that?
        if(PHYSFS_isDirectory((dir+filename).c_str()) == onlyFiles)
        {
            //it's a file and we want directories or vice versa - abort
            return;
        }
        set.insert(filename);
    }

    //callback function for directory enumeration
    static void AddDirToSet(void* setPtrVoid, const char* dir, const char* file)
    {
        AddToSet(*static_cast<caseInsStringSet* >(setPtrVoid), dir, file, false);
    }

    //callback function for file enumeration
    static void AddFileToSet(void* vecPtrVoid, const char* dir, const char* file)
    {
        AddToSet(*static_cast<caseInsStringSet* >(vecPtrVoid), dir, file, true);
    }

    //luabind only gets the iterators of return_stl_iterators functions, i.e. the vector itself is not saved, thus the iterators don't work anymore unless I've got a vector that still exists
    static std::vector<std::string> g_stringVec;
}

const std::vector<std::string>& GetFilesInDirectory(const std::string& dir)
{
    caseInsStringSet set;
    PHYSFS_enumerateFilesCallback(dir.c_str(), AddFileToSet, static_cast<void*>(&set));
    g_stringVec.clear();
    for(caseInsStringSet::iterator it = set.begin(); it != set.end(); ++it)
    {
        g_stringVec.push_back(*it);
    }
    return g_stringVec;
}

const std::vector<std::string>& GetDirectoriesInDirectory(const std::string& dir)
{
    caseInsStringSet set;
    PHYSFS_enumerateFilesCallback(dir.c_str(), AddDirToSet, static_cast<void*>(&set));
    g_stringVec.clear();
    for(caseInsStringSet::iterator it = set.begin(); it != set.end(); ++it)
    {
        g_stringVec.push_back(*it);
        //Logger::GetDefaultLogger().Log(*it);
    }
    return g_stringVec;
}

}
}
