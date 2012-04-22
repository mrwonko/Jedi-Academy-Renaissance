#ifndef JAR_AUDIO_HPP
#define JAR_AUDIO_HPP

#include "jar/audio/API.hpp"
#include <cassert>

namespace jar {

class AudioImpl;

class JARAUDIOAPI Audio
{
    public:
        Audio();
        virtual ~Audio();

        static Audio& GetSingleton() { assert(mSingleton); return *mSingleton; }

    private:
        AudioImpl* mImpl;
        static Audio* mSingleton;
};

} // namespace jar

#endif // JAR_INPUT_INPUT_HPP
