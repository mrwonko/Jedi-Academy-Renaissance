#include "jar/Input.hpp"
#include "jar/input/InputImpl.hpp"

namespace jar {

Input* Input::mSingleton = NULL;

Input::Input() :
    mImpl(new InputImpl)
{
    assert(mImpl);
    assert(!mSingleton);
    mSingleton = this;
}

Input::~Input()
{
    delete mImpl;
    assert(mSingleton);
    mSingleton = NULL;
}

InputDeviceManager& Input::GetInputDeviceManager()
{
    return mImpl->GetInputDeviceManager();
}

EventManager& Input::GetEventManager()
{
    return mImpl->GetEventManager();
}

void Input::OnWindowCreated()
{
    mImpl->OnWindowCreated();
}

} // namespace jar
