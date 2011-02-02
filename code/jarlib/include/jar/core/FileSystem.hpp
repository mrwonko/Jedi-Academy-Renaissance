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

#ifndef JAR_FILESYSTEM_HPP
#define JAR_FILESYSTEM_HPP

#include <stdint.h> //int64_t
#include <string>
#include <vector>
#include <jar/core/API.hpp>

//This file contains some wrapper functions for PhysFS function so the user doesn't need to have the PhysFS libraries to use jarlib.
//It also changes the functionality of some functions to add prefixes etc. so they work as expected.

//forward declaration of internally used file type.
struct PHYSFS_File;

namespace jar
{
namespace fs
{
typedef PHYSFS_File* File;

/** \brief Initializes the file system. Call this first. **/
JARCOREAPI const bool Init(char* argv0);

/** \brief If this returns false, something is probably badly screwed up. **/
JARCOREAPI const bool Deinit();

JARCOREAPI const bool SetWriteDir(const std::string& writedir);

JARCOREAPI const std::string GetLastError();

/** \brief Opens and reads a given file into the output parameter, then closes it.
    \return success **/
JARCOREAPI const bool ReadFile(const std::string& filename, std::string& output);

/** \brief Closes a given file. **/
JARCOREAPI const bool Close(fs::File file);

/** \brief Opens a given file for reading. Returns NULL on error. **/
JARCOREAPI fs::File OpenRead(const std::string& filename);

/** \brief Opens a given file for writing (clearing it if it exists), which is always done in the writepath. Returns NULL on error. **/
JARCOREAPI fs::File OpenWrite(const std::string& filename);

/** \brief Opens a given file for writing (appending to any existing content), which is always done in the writepath. Returns NULL on error. **/
JARCOREAPI fs::File OpenAppend(const std::string& filename);

/** \brief Completely reads the given file into the output string.

    \return Success. Check error via GetLastError() on failure.
**/
JARCOREAPI const bool GetCurrentFileContent(fs::File file, std::string& output);

/** \brief Mounts a given file/directory to /
    \note Needs CLArguments
    \param append true = try later (last appended = last resort)
    \return success **/
JARCOREAPI const bool Mount(const std::string& filename, const bool append);

/** \brief Mounts a given file/directory to a given mountpoint
    \note Needs CLArguments
    \param append true = try later (last appended = last resort)
    \return success **/
JARCOREAPI const bool Mount(const std::string& filename, const std::string& mountpoint, const bool append);

/** \brief Unmounts a given file/directory (that's been mounted first)
    \return success **/
JARCOREAPI const bool Unmount(const std::string& filename);

/** \brief Reads an int from a given file.
    \note Little endian **/
JARCOREAPI const bool ReadInt(fs::File file, int& out_num);

/** \brief Reads an unsigned int from a given file.
    \note Little endian **/
JARCOREAPI const bool ReadUnsignedInt(fs::File file, unsigned int& out_num);

/** \brief Reads a char from a given file. **/
JARCOREAPI const bool ReadChar(fs::File file, char& output);

/** \brief Reads a float from a given file.**/
JARCOREAPI const bool ReadFloat(fs::File file, float& out_num);

/** \brief Reads a null-terminated string from a given file, or until eof. **/
JARCOREAPI const bool ReadString(fs::File file, std::string& output);

/** \brief Reads a string of a given length from a file, stopping at eof. **/
JARCOREAPI const bool ReadString(fs::File file, unsigned int len, std::string& output);

/** \brief Reads a given number of objectfrom a file
    file file handle returned from PHYSFS_openRead().
	buffer buffer to store read data into.
	objSize size in bytes of objects being read from (file).
	objCount number of (objSize) objects to read from (file).
    \return number of objects read. GetLastError() can shed light on the reason this might be < (objCount), as can EndOfFile(). -1 if complete failure. **/
JARCOREAPI const int Read(fs::File file, void* buffer, unsigned int objSize, unsigned int objCount);

/** \brief Returns whether a file's eof has been reached yet.
    \note I can't use PHYSFS_eof since it returns an int, and to Lua 0 = true. **/
JARCOREAPI const bool EndOfFile(fs::File file);

/** \brief Writes a string to a file
    \note File needs to be opened via physfs.OpenWrite() **/
JARCOREAPI const bool WriteString(fs::File file, const std::string& str);

/** \brief Moves cursor to a given position in the file.
    \return success **/
JARCOREAPI const bool Seek(fs::File file, const int64_t position);

/** \brief returns the current position of the cursor. **/
JARCOREAPI const int64_t Tell(fs::File file);


/** \brief returns all the files within a given directory, ordered by filename (case sensitive?). **/
JARCOREAPI const std::vector<std::string>& GetFilesInDirectory(const std::string& dir);

/** \brief returns all the directories within a given directory, ordered by filename (case sensitive?). **/
JARCOREAPI const std::vector<std::string>& GetDirectoriesInDirectory(const std::string& dir);

}
}

#endif // JAR_FILESYSTEM_HPP
