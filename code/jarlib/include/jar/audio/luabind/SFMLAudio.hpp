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

#ifndef JAR_AUDIO_LUABIND_SFMLAUDIO_HPP
#define JAR_AUDIO_LUABIND_SFMLAUDIO_HPP

#include <jar/audio/API.hpp>
#include <string>
//forward declarations
struct lua_State;
namespace sf
{
    class SoundBuffer;
    class Music;
}

namespace jar
{

/** \brief Binds some SFML/Audio classes to Lua via luabind.
**/
void JARAUDIOAPI BindSFMLAudio(lua_State* L);
const bool JARAUDIOAPI SoundBufferLoadFromFile(sf::SoundBuffer& buf, const std::string& filename);
//const bool JARAUDIOAPI MusicOpenFromFile(sf::Music& mus, const std::string& filename);
}

#endif
