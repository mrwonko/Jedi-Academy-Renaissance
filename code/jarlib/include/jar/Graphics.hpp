#ifndef JAR_GRAPHICS_HPP
#define JAR_GRAPHICS_HPP

#include <jar/core/Singleton.hpp>
#include <jar/graphics/API.hpp>

namespace jar {

class GraphicsImpl;

class JARGRAPHICSAPI Graphics : public Singleton<Graphics>
{
    public:
        Graphics();
        virtual ~Graphics();

    protected:

    private:
        GraphicsImpl* mImpl;
};

//template<> JARGRAPHICSAPI Graphics* Singleton<Graphics>::mSingleton;

} // namespace jar

#endif // JAR_GRAPHICS_GRAPHICS_HPP
