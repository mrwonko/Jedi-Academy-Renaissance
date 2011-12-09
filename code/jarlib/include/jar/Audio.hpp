#ifndef JAR_AUDIO_HPP
#define JAR_AUDIO_HPP

#include "jar/core/Singleton.hpp"
#include "jar/audio/API.hpp"

namespace jar {

class AudioImpl;

class JARAUDIOAPI Audio : public Singleton<Audio>
{
    public:
        Audio();
        virtual ~Audio();

    private:
        AudioImpl* mImpl;
};

template<> JARAUDIOAPI Audio* Singleton<Audio>::mSingleton;

} // namespace jar

#endif // JAR_INPUT_INPUT_HPP
