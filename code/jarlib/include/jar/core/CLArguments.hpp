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

//  jar::CLArguments
//
//  a class for dealing with command line arguments passed to the program
//
//  Last modified 2010-10-22 by Willi

#ifndef JAR_CORE_CLARGUMENTS_HPP
#define JAR_CORE_CLARGUMENTS_HPP
#include <string>
#include <vector>
#include "Singleton.hpp"
#include <jar/core/API.hpp>

namespace jar
{
    /**
     *  \brief a class keeping track of the <b>C</b>ommand <b>L</b>ine <b>Arguments</b> the program got at startup
     *
     *  Also parses the first argument to get the path to the program
     *  \headerfile <jar/core/CLArguments.hpp>
     **/
    class JARCOREAPI CLArguments : public Singleton<CLArguments>
    {
        public:
            /**
            *
            *   \brief constructor for the argument thingy
            *
            *   \param argc number of arguments given, equals the first parameter of main
            *   \param argv arguments given, equals the second parameter of main
            *   \bug won't work correctly for paths with mixed / and \\
            *
            **/
            CLArguments(int argc, char** argv);


            /**
            *
            *   \brief returns the directory containing the executable
            *
            *   \return the directory containing the executable
            *
            **/
            const std::string& GetWorkingDirectory() const { return mWorkingDir; }

            /** \brief returns the path to the executable of the program
             *
             * \return  path to executable
             */
            const std::string& GetExecutable() const { return mExecutable; }


            /**
            *
            *   \brief constructor for the argument thingy
            *
            *   \param argc number of arguments given, equals the first parameter of main
            *   \param argv arguments given, equals the second parameter of main
            *
            **/
            std::string MatchPath(const std::string& dir) const;

            /**
             *
             *  \brief gives you all arguments
             *
             *  not including the first argument which is the path.
             *
             **/
            const std::vector <std::string>& GetArguments() const { return mArguments; }

            /**
             *  \brief Returns all the arguments concatenated to a long string.
             **/
            const std::string& GetArgumentString() const {return mArgumentString; }

            const std::string& GetArgument(const int index) const { return mArguments.at(index); }

            /**
             *  \brief  Finds a given argument
             *  \param  caseSensitive   whether "ARG" differs from "arg", default false
             *  \return position or -1 if the argument does not exist
             */
            const int FindArgument(const std::string& requestedArgument, bool caseSensitive = false) const;

            const int NumArguments() const { return mArguments.size(); }
        private:
            /// All the arguments (except the path)
            std::vector < std::string > mArguments;

            /// The path to the executable
            std::string mWorkingDir;
            /// First argument, the executable + path
            std::string mExecutable;

            /// All the arguments concatenated
            std::string mArgumentString;

            /// Whether this is windows. Unreliable! Only checks for \\ in path, which may not be present on windows.
            bool mWindows;
    };

    template<> JARCOREAPI CLArguments* Singleton<CLArguments>::mSingleton;

}
#endif // JAR_CORE_CLARGUMENTS_HPP
