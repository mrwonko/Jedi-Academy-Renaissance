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

#ifndef JAR_CORE_HPP
#define JAR_CORE_HPP

#include <string>
#include "core/Singleton.hpp"
#include <jar/core/API.hpp>
#include <jar/core/Time.hpp>

namespace jar {
//forward declarations
class CoreImpl;
class Lua;

/** \brief Main Framework class for core systems. You, the user, just need to create & initialize this (only once!), plus any other systems you need. **/
class JARCOREAPI Core : public Singleton<Core>
{
    public:
        Core();
        ~Core();

        /** \brief Initializes the whole framework.
         * \param argc first parameter of main()
         * \param argv second parameter of main()
         * \param rootPath path to application's root folder, relative to binary
         * \return Success */
        const bool Init(int argc, char** argv, const std::string& rootPath);
        const bool Deinit();

        void Update(const TimeType deltaT);

        const std::string& GetRootPath();

        Lua& GetLua();

        CoreImpl& GetImpl();

    private:
        /** \brief Pointer to implementation of this class, so user doesn't need to include all the files. **/
        CoreImpl* mImpl;
};

template<> JARCOREAPI Core* Singleton<Core>::mSingleton;

} // namespace jar

#endif // JAR_CORE_HPP
