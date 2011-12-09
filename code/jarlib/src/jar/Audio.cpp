#include "jar/Audio.hpp"
#include "jar/audio/AudioImpl.hpp"
#include <cstddef>

namespace jar {

template<> Audio* Singleton<Audio>::mSingleton = NULL;

Audio::Audio() :
    mImpl(new AudioImpl)
{
    assert(mImpl);
}

Audio::~Audio()
{
    delete mImpl;
}

} // namespace jar
