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

#include "jar/audio/luabind/SFMLAudio.hpp"

#include "jar/core/FileSystem.hpp"

#include "SFML/Audio/Listener.hpp"
#include "SFML/Audio/SoundSource.hpp"
#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Audio/SoundStream.hpp"
#include "SFML/Audio/Music.hpp"

//TODO: Music requires some more work to make streaming work with PhysFS, do that later.
//#include "SFML/Audio/Music.hpp"

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/dependency_policy.hpp>

namespace jar
{

const bool SoundBufferLoadFromFile(sf::SoundBuffer& buf, const std::string& filename)
{
    std::string content;
    if(!fs::ReadFile(filename, content) &&
       !fs::ReadFile(filename + ".wav", content) &&
       !fs::ReadFile(filename + ".mp3", content) &&
       !fs::ReadFile(filename + ".ogg", content)
       ) return false;
    return buf.LoadFromMemory(content.c_str(), content.length());
}

const bool MusicOpenFromFile(sf::Music& mus, const std::string& filename)
{
    std::string content;
    if(!fs::ReadFile(filename, content) &&
       !fs::ReadFile(filename + ".wav", content) &&
       !fs::ReadFile(filename + ".mp3", content) &&
       !fs::ReadFile(filename + ".ogg", content)
       ) return false;
    return mus.OpenFromMemory(content.c_str(), content.length());
}

void BindSFMLAudio(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<sf::Listener>("Listener")
            .scope
            [
                luabind::def("SetGlobalVolume", sf::Listener::SetGlobalVolume),
                luabind::def("GetGlobalVolume", sf::Listener::GetGlobalVolume),
                luabind::def("SetPosition", (void(*)(float, float, float))sf::Listener::SetPosition),
                //TODO: expose Vector3f first!
                //luabind::def("GetPosition", sf::Listener::GetPosition),
                luabind::def("SetDirection", (void(*)(float, float, float))sf::Listener::SetDirection)
                //TODO: expose Vector3f first!
                //luabind::def("GetDirection", sf::Listener::GetDirection),
            ],

        luabind::class_<sf::SoundSource>("SoundSource")
            .enum_("Status")
            [
                luabind::value("Stopped", sf::SoundSource::Stopped),
                luabind::value("Paused", sf::SoundSource::Paused),
                luabind::value("Playing", sf::SoundSource::Playing)
            ]
            .def("SetPitch", &sf::SoundSource::SetPitch)
            .def("GetPitch", &sf::SoundSource::GetPitch)
            .def("SetVolume", &sf::SoundSource::SetVolume)
            .def("GetVolume", &sf::SoundSource::GetVolume)
            .def("SetAttentuation", &sf::SoundSource::SetAttenuation)
            .def("GetAttentuation", &sf::SoundSource::GetAttenuation)
            .def("SetPosition", (void(sf::SoundSource::*)(float, float, float))&sf::SoundSource::SetPosition)
            //TODO: expose Vector3f first!
            //luabind::def("GetPosition", &sf::SoundSource::GetPosition),
            .def("SetRelativeToListener", &sf::SoundSource::SetRelativeToListener)
            .def("IsRelativeToListener", &sf::SoundSource::IsRelativeToListener)
            .def("SetMinDistance", &sf::SoundSource::SetMinDistance)
            .def("GetMinDistance", &sf::SoundSource::GetMinDistance),

        luabind::class_<sf::SoundBuffer>("SoundBuffer")
            .def(luabind::constructor<>())
            .def("LoadFromFile", SoundBufferLoadFromFile),

        luabind::class_<sf::Sound, sf::SoundSource>("Sound")
            .def(luabind::constructor<>())
            .def("Play", &sf::Sound::Play)
            .def("Pause", &sf::Sound::Pause)
            .def("Stop", &sf::Sound::Stop)
            .def("SetBuffer", &sf::Sound::SetBuffer, luabind::dependency(_1, _2)) //1st: who keeps alive, 2nd: who is kept alive. _1 = this.
            .def("GetBuffer", &sf::Sound::GetBuffer)
            .def("SetLoop", &sf::Sound::SetLoop)
            .def("GetLoop", &sf::Sound::GetLoop)
            .def("SetPlayingOffset", &sf::Sound::SetPlayingOffset)
            .def("GetPlayingOffset", &sf::Sound::GetPlayingOffset)
            .def("GetStatus", &sf::Sound::GetStatus),

        luabind::class_<sf::SoundStream, sf::SoundSource>("SoundStream")
            .def("Play", &sf::SoundStream::Play)
            .def("Pause", &sf::SoundStream::Pause)
            .def("Stop", &sf::SoundStream::Stop)
            .def("SetLoop", &sf::SoundStream::SetLoop)
            .def("GetLoop", &sf::SoundStream::GetLoop)
            .def("GetStatus", &sf::SoundStream::GetStatus)
            .def("SetPlayingOffset", &sf::SoundStream::SetPlayingOffset)
            .def("GetPlayingOffset", &sf::SoundStream::GetPlayingOffset),

        luabind::class_<sf::Music, sf::SoundStream>("Music")
            .def(luabind::constructor<>())
            .def("OpenFromFile", MusicOpenFromFile)
            .def("GetDuration", &sf::Music::GetDuration)
    ];
}

}
