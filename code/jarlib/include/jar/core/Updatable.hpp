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

#ifndef JAR_CORE_UPDATABLE_HPP
#define JAR_CORE_UPDATABLE_HPP

#include "jar/core/Time.hpp"

namespace jar
{

/** \brief Automatically gets updated every frame. (If Core gets updated, that is.) **/
class JARCOREAPI Updatable
{
    public:
        Updatable();
        virtual ~Updatable();
        virtual void Update(const TimeType deltaT);
};

} // namespace jar

#endif // JAR_CORE_UPDATABLE_HPP
