#include "jar/Input.hpp"
#include "jar/input/InputImpl.hpp"

namespace jar {

template<> Input* Singleton<Input>::mSingleton = NULL;

Input::Input() :
    mImpl(new InputImpl)
{
    assert(mImpl);
}

Input::~Input()
{
    delete mImpl;
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
