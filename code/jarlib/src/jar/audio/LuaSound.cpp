#include "jar/audio/LuaSound.hpp"
#include <luabind/class.hpp>

namespace jar
{

LuaSound::LuaSound(const LuaSound& copy) :
	sf::Sound( copy )
{
	mLuaSoundBuffer = copy.mLuaSoundBuffer;
}

void LuaSound::BindToLua(lua_State* L)
{
	luabind::module( L, "jar" )
	[
		luabind::class_<LuaSound, sf::SoundSource>("Sound")
			.def(luabind::constructor<>())
			.def("Play", &sf::Sound::play)
			.def("Pause", &sf::Sound::pause)
			.def("Stop", &sf::Sound::stop)
			.def("SetBuffer", &SetBuffer)
			.def("GetBuffer", &GetBuffer)
			.def("SetLoop", &sf::Sound::setLoop)
			.def("GetLoop", &sf::Sound::getLoop)
			.def("SetPlayingOffset", &sf::Sound::setPlayingOffset)
			.def("GetPlayingOffset", &sf::Sound::getPlayingOffset)
			.def("GetStatus", &sf::Sound::getStatus)
		];
}

luabind::object LuaSound::GetBuffer()
{
	return mLuaSoundBuffer;
}

void LuaSound::SetBuffer( const luabind::object& bufferObj )
{
	// will throw if impossible, which is just fine by me.
	const sf::SoundBuffer* buffer = luabind::object_cast< const sf::SoundBuffer* > ( bufferObj );
	mLuaSoundBuffer = bufferObj;
	sf::Sound::setBuffer( *buffer );
}

} // namespace jar
