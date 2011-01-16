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

#ifndef JAR_JARIMPL_HPP
#define JAR_JARIMPL_HPP

#include "jar/core/Time.hpp"
#include <string>
#include <set>
#include <cassert>

namespace jar {

//forward declarations
class CLArguments;
class Lua;
class Component;
class Updatable;

/** \brief Implementation of the Jar class **/
class CoreImpl
{
    public:
        CoreImpl();
        ~CoreImpl();

        const bool Init(int argc, char** argv, const std::string& rootPath);
        const bool Deinit();

        const std::string& GetRootPath() { return mRootPath; }

        void Update(const TimeType deltaT);

        Lua& GetLua() { assert(mLua); return *mLua;}

        void RegisterComponent(Component* c);
        void UnregisterComponent(Component* c);

        void RegisterUpdatable(Updatable* u);
        void UnregisterUpdatable(Updatable* u);

    private:
        CLArguments* mCLArguments;
        /** \brief Relative path from binaries to root folder **/
        std::string mRootPath;
        Lua* mLua;
        bool mPhysFSInitialized;

        std::set<Component*> mComponents;
        std::set<Updatable*> mUpdatables;
};

} // namespace jar

#endif // JAR_JARIMPL_HPP
