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
