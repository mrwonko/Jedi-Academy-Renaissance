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

#include "jar/core/Updatable.hpp"
#include "jar/Core.hpp"
#include "jar/core/CoreImpl.hpp"

#warning Export Updatable to Lua (inheritable), ought to be quite useful!

namespace jar {

Updatable::Updatable()
{
    //ctor
    assert(Core::HasSingleton());
    Core::GetSingleton().GetImpl().RegisterUpdatable(this);
}

Updatable::~Updatable()
{
    //dtor
    if(Core::HasSingleton())
    {
        Core::GetSingleton().GetImpl().UnregisterUpdatable(this);
    }
}

void Updatable::Update(const TimeType deltaT)
{
    //some people may forget overwriting the Update() function, or don't need it but have to inherit Updatable (indirectly)
}

} // namespace jar
