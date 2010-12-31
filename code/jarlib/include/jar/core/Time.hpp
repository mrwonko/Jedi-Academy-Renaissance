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

#ifndef JAR_CORE_TIME_HPP
#define JAR_CORE_TIME_HPP

#include "jar/core/API.hpp"

namespace jar
{

/// Conversion from seconds to TimeType
const float TimeTypesPerSecond = 1000;
/// Conversion from TimeType to seconds
const float SecondsPerTimeType = 1.0f / TimeTypesPerSecond;

/// The type of time variables
typedef long int TimeType; //when you change this, check GetTime() for validity!

JARCOREAPI TimeType GetTime();

} //namespace jar

#endif //JAR_CORE_TIME_HPP
