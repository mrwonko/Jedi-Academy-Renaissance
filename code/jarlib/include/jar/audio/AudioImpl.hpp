#ifndef JAR_AUDIO_AUDIOIMPL_HPP
#define JAR_AUDIO_AUDIOIMPL_HPP

#include "jar/core/Component.hpp"

namespace jar {

class AudioImpl : public Component
{
    public:
        AudioImpl();
        virtual ~AudioImpl();

        virtual const bool Init();

        virtual const bool Deinit();

    private:
};

} // namespace jar

#endif
