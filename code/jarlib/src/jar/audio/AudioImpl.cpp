#include "jar/audio/luabind/SFMLAudio.hpp"
#include "jar/audio/AudioImpl.hpp"
#include "jar/audio/LuaSound.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Lua.hpp"
#include "jar/Core.hpp"
namespace jar {

AudioImpl::AudioImpl()
{
    //ctor
}

AudioImpl::~AudioImpl()
{
    //dtor
}

const bool AudioImpl::Init()
{
    Logger::GetDefaultLogger().Info("- Initializing Audio component...", 1);

    Logger::GetDefaultLogger().Info("Exposing Audio System to Lua...", 2);
	lua_State* L = Core::GetSingleton().GetLua().GetState();
    BindSFMLAudio(L);
	LuaSound::BindToLua(L);

    Logger::GetDefaultLogger().Info("- Audio component initialized...", 1);

    return true;
}

const bool AudioImpl::Deinit()
{
    return true;
}

} // namespace jar
