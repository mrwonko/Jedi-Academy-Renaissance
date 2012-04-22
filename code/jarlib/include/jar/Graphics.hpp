#ifndef JAR_GRAPHICS_HPP
#define JAR_GRAPHICS_HPP

#include <jar/graphics/API.hpp>
#include <cassert>

namespace jar {

class GraphicsImpl;

class JARGRAPHICSAPI Graphics
{
    public:
        Graphics();
        virtual ~Graphics();

        static Graphics& GetSingleton() { assert(mSingleton); return *mSingleton; }
    protected:

    private:
        GraphicsImpl* mImpl;
        static Graphics* mSingleton;
};

} // namespace jar

#endif // JAR_GRAPHICS_GRAPHICS_HPP
