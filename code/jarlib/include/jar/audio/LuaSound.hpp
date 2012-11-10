#ifndef JAR_LUA_SOUND_HPP
#define JAR_LUA_SOUND_HPP

#include "SFML/Audio/Sound.hpp"
#include <luabind/object.hpp>

struct lua_State;

namespace jar
{
	/**
		@brief Pretty much sf::Sound, except for usage from within Lua

		Keeps its SoundBuffer alive.

		Caution: Only use from Lua! Calling sf::Sound::SetBuffer will keep the Lua one alive unnecessarily.
	**/
	class LuaSound : public sf::Sound
	{
	public:
		LuaSound() : sf::Sound() {}
		LuaSound(const LuaSound& copy);

		static void BindToLua( lua_State* L );

	private:
		luabind::object GetBuffer();

		void SetBuffer( const luabind::object& buffer );

		luabind::object mLuaSoundBuffer;
	};
}

#endif
