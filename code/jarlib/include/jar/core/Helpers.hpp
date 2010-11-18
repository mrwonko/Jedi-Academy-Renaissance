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

#ifndef JAR_CORE_HELPERS_HPP
#define JAR_CORE_HELPERS_HPP

#include <string>
#include <vector>
#include <jar/Library.hpp>

namespace jar
{
    /** \brief A class containing a couple of (static) helper methods.
        \headerfile <jar/core/Helpers.hpp>
    **/
	class JARAPI Helpers
	{
    private:
		/**
		 * \brief all the methods are static, no need to construct objects.
		 **/
		Helpers() {}

    public:
		/**
		 *
		 *  \brief  removes spaces and tabs from the beginning and the end of a string
		 *
		 *  \param  string  string to trim
		 *
		 *  \return string without spaces and tabs at beginning and end
		 *
		 **/
		static std::string Trim(const std::string& str);

		/**
		 *
		 *  \brief  converts an integer to a string
		 *
		 *  \param  integer the integer to convert
		 *
		 *  \return string containing the number
		 *
		 *  \see FloatToString
		 *  \see StringToInt
		 *  \see StringToFloat
		 *
		 **/
		static std::string IntToString(int integer);

		/**
		 *
		 *  \brief  converts a float to a string
		 *
		 *  \param  number the float to convert
		 *
		 *  \return string containing the number
		 *
		 *  \see IntToString
		 *  \see StringToInt
		 *  \see StringToFloat
		 *
		 **/
		static std::string FloatToString(float number);

		/**
		 *
		 *  \brief  converts a string to an integer
		 *
		 *  \param  string  string to convert
		 *
		 *  \return string as an int
		 *
		 *  \see StringToFloat
		 *  \see FloatToString
		 *  \see IntToString
		 *
		 **/
		static int StringToInt(const std::string& str);

		/**
		 *
		 *  \brief  converts an integer to a string
		 *
		 *  \param  integer the integer to convert
		 *
		 *  \return string containing the number
		 *
		 *  \see StringToInt
		 *  \see FloatToString
		 *  \see IntToString
		 *
		 **/
		static float StringToFloat(const std::string& str);

		/**
		 *
		 *  \brief  removes c-style comments from a given string
		 *
		 *  C-Style means // and / * * / style
		 *
		 *  \param  textWithComments the text containing comments
		 *
		 *  \return the text without the comments
		 *
		 **/
		static std::string RemoveCStyleComments(const std::string& textWithComments);

		/// \brief Converts a string to lower case
		static std::string ToLower(const std::string& text);

		/**
		 *  \brief replaces something in a string with something else.
		 *  \param  text    the text in which to search
		 *  \param  pattern the text to find
		 *  \param  replace the text to replace the matches with
		 *  \bug    even if the text in question appears within a string it will be treated as a normal comment. e.g. "a comment looks like this: // comment" will have the end removed.
		 *  \return the string with everything replaced
		 **/
		static std::string Replace(const std::string& text, const std::string& pattern, const std::string& replace);

		/**
		 *	\brief rounds a given float
		 **/
		static int Round(float f);

		/** \brief Returns all files in a given directory, relative to the g_rootDir
            \return vector of filenames (without path)
		 **/
		 static std::vector<std::string> GetFilesInDirectory(std::string directoryPath);

		/** \brief Returns all directories in a given directory except those starting with ., relative to the g_rootDir
            \return vector of filenames (without path)
		 **/
		 static std::vector<std::string> GetDirectoriesInDirectory(std::string directoryPath);
	};
}

#endif // JAR_CORE_HELPERS_HPP
