#include "jar/Graphics.hpp"
#include "jar/graphics/GraphicsImpl.hpp"
#include <cstddef>

namespace jar {

Graphics* Graphics::mSingleton = NULL;

Graphics::Graphics()
{
    //ctor
    mImpl = new GraphicsImpl();
    assert(!mSingleton);
    mSingleton = this;
}

Graphics::~Graphics()
{
    //dtor
    delete mImpl;
    assert(mSingleton);
    mSingleton = NULL;
}

} // namespace jar
