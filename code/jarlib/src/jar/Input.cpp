#include "jar/Input.hpp"
#include "jar/input/InputImpl.hpp"

namespace jar {

template<> Input* Singleton<Input>::mSingleton = 0;

Input::Input() :
    mImpl(new InputImpl)
{
    assert(mImpl);
}

Input::~Input()
{
    delete mImpl;
}

} // namespace jar
