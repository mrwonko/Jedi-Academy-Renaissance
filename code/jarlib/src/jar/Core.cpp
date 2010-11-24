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

#include "jar/Core.hpp"
#include "jar/core/CoreImpl.hpp"

namespace jar {

template<> Core* Singleton<Core>::mSingleton = NULL;

Core::Core() :
    mImpl(new CoreImpl)
{
    assert(mImpl);
}

Core::~Core()
{
    delete mImpl;
}

const bool Core::Init(int argc, char** argv, const std::string& rootPath)
{
    return mImpl->Init(argc, argv, rootPath);
}

const bool Core::Deinit()
{
    return mImpl->Deinit();
}

const std::string& Core::GetRootPath()
{
    return mImpl->GetRootPath();
}

Lua& Core::GetLua()
{
    return mImpl->GetLua();
}

CoreImpl& Core::GetImpl()
{
    assert(mImpl);
    return *mImpl;
}

void Core::Update(const TimeType deltaT)
{
    mImpl->Update(deltaT);
}

} // namespace jar
