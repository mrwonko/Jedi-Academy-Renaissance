#ifndef JAR_GRAPHICS_GRAPHICSIMPL_HPP
#define JAR_GRAPHICS_GRAPHICSIMPL_HPP

#include "jar/core/Component.hpp"

namespace jar {

class GraphicsImpl : public Component
{
    public:
        GraphicsImpl();
        virtual ~GraphicsImpl();

        virtual const bool Init();
        virtual const bool Deinit();

    protected:
    private:
};

} // namespace jar

#endif // JAR_GRAPHICS_GRAPHICSIMPL_HPP
