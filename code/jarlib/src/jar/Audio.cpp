#include "jar/Audio.hpp"
#include "jar/audio/AudioImpl.hpp"
#include <cstddef>

namespace jar {

Audio* Audio::mSingleton = NULL;

Audio::Audio() :
    mImpl(new AudioImpl)
{
    assert(mImpl);
    assert(!mSingleton);
    mSingleton = this;
}

Audio::~Audio()
{
    delete mImpl;
    assert(mSingleton);
    mSingleton = NULL;
}

} // namespace jar
