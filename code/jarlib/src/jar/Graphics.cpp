#include "jar/Graphics.hpp"
#include "jar/graphics/GraphicsImpl.hpp"
#include <cstddef>

namespace jar {

template<> Graphics* Singleton<Graphics>::mSingleton = NULL;

Graphics::Graphics()
{
    //ctor
    mImpl = new GraphicsImpl();
}

Graphics::~Graphics()
{
    //dtor
    delete mImpl;
}

} // namespace jar
