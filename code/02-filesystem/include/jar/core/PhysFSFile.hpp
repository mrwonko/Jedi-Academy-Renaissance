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

#ifndef JAR_PHYSFSFILE_HPP
#define JAR_PHYSFSFILE_HPP

#include <string>

struct PHYSFS_File;

namespace jar
{

/** \brief Completely reads the given file into the output string.

    \return Success. Check error via PHYSFS_getLastError() on failure.
**/
const bool PhysFSGetCurrentFileContent(PHYSFS_File* file, std::string& output);

/** \brief Mounts a given file/directory to /
    \note Needs CLArguments
    \param append true = try later (last appended = last resort)
    \return success **/
const bool PhysFSMount(const std::string& filename, const bool append);

/** \brief Mounts a given file/directory to a given mountpoint
    \note Needs CLArguments
    \param append true = try later (last appended = last resort)
    \return success **/
const bool PhysFSMount(const std::string& filename, const std::string& mountpoint, const bool append);

/** \brief Unmounts a given file/directory (that's been mounted first)
    \return success **/
const bool PhysFSUnmount(const std::string& filename);

/** \brief Reads an int from a given file.
    \note Little endian **/
const bool PhysFSReadInt(PHYSFS_File* file, int& out_num);

/** \brief Reads an unsigned int from a given file.
    \note Little endian **/
const bool PhysFSReadUnsignedInt(PHYSFS_File* file, unsigned int& out_num);

/** \brief Reads a char from a given file. **/
const bool PhysFSReadChar(PHYSFS_File* file, char& output);

/** \brief Reads a float from a given file.**/
const bool PhysFSReadFloat(PHYSFS_File* file, float& out_num);

/** \brief Reads a null-terminated string from a given file, or until eof. **/
const bool PhysFSReadString(PHYSFS_File* file, std::string& output);

/** \brief Reads a string of a given length from a file, stopping at eof. **/
const bool PhysFSReadString(PHYSFS_File* file, unsigned int len, std::string& output);

/** \brief Returns whether a file's eof has been reached yet.
    \note I can't use PHYSFS_eof since it returns an int, and to Lua 0 = true. **/
const bool PhysFSEOF(PHYSFS_File* file);

/** \brief Writes a string to a file
    \note File needs to be opened via physfs.OpenWrite() **/
const bool PhysFSWriteString(PHYSFS_File* file, const std::string& str);

/** \brief Moves cursor to a given position in the file.
    \return success **/
const bool PhysFSSeek(PHYSFS_File* file, const int64_t position);

}

#endif // JAR_PHYSFSFILE_HPP
