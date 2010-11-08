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

/** \brief The directory of the mod, i.e. where data files are, relative to binary.
    \todo define elsewhere! **/
const std::string MODDIR("../");

/** \brief Completely reads the given file into the output string.

    \return Success. Check error via PHYSFS_getLastError() on failure.
**/
const bool PhysFSGetCurrentFileContent(PHYSFS_File* file, std::string& output);

/** \brief Mounts a given file/directory to /
    \note Needs CLArguments
    \param append true = try later (last appended = last resort) **/
const int PhysFSMount(const std::string& filename, const bool append);

/** \brief Mounts a given file/directory to a given mountpoint
    \note Needs CLArguments
    \param append true = try later (last appended = last resort) **/
const int PhysFSMount(const std::string& filename, const std::string& mountpoint, const bool append);

/** \brief Reads an int from a given file.
    \note Little endian **/
const int PhysFSReadInt(PHYSFS_File* file);

/** \brief Reads an unsigned int from a given file.
    \note Little endian **/
const unsigned int PhysFSReadUnsignedInt(PHYSFS_File* file);

/** \brief Reads a char from a given file. **/
const char PhysFSReadChar(PHYSFS_File* file);

/** \brief Reads a float from a given file.**/
const float PhysFSReadFloat(PHYSFS_File* file);

/** \brief Reads a null-terminated string from a given file, or until eof. **/
std::string PhysFSReadString(PHYSFS_File* file);

/** \brief Reads a string of a given length from a file, stopping at eof. **/
std::string PhysFSReadString(PHYSFS_File* file, unsigned int len);

/** \brief Returns whether a file's eof has been reached yet.
    \note I can't use PHYSFS_eof since it returns an int, and to Lua 0 = true. **/
const bool PhysFSEOF(PHYSFS_File* file);

/** \brief Writes a string to a file
    \note File needs to be opened via physfs.OpenWrite() **/
const bool PhysFSWriteString(PHYSFS_File* file, const std::string& str);

}

#endif // JAR_PHYSFSFILE_HPP
