#include "jar/audio/luabind/SFMLAudio.hpp"

#include "jar/core/FileSystem.hpp"

#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundStream.hpp>
#include <SFML/Audio/Music.hpp>

//TODO: Music requires some more work to make streaming work with PhysFS, do that later.
#include <SFML/Audio/Music.hpp>

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

/*
//this requires the memory to remain intact, since it's used for streaming. I'm not doing this.
const bool MusicOpenFromFile(sf::Music& mus, const std::string& filename)
{
    std::string* content = new std::string();
    return
    (
        (
            fs::ReadFile(filename, *content) ||
            fs::ReadFile(filename + ".wav", *content) ||
            fs::ReadFile(filename + ".mp3", *content) ||
            fs::ReadFile(filename + ".ogg", *content)
        )
        &&
        mus.OpenFromMemory(content->c_str(), content->length())
     );
}
*/

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
            .def("GetStatus", &sf::Sound::GetStatus)
/*
        //music requires the memory from where it is read to remain intact, I don't want to deal with that at the moment, would require a manager of sorts or inheriting for PhysFS support.
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
            */
    ];
}

}
