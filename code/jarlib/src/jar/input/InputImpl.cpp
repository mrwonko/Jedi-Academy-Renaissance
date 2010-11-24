#include "jar/input/InputImpl.hpp"
#include "jar/input/InputDeviceManager.hpp"

#ifdef _WIN32
#include "jar/input/Windows/WinKeyboard.hpp"
#endif

namespace jar {

InputImpl::InputImpl()
{
    //ctor
}

InputImpl::~InputImpl()
{
    //dtor
}

const bool InputImpl::Init()
{
    mInputDeviceManager = new InputDeviceManager();
    if(!mInputDeviceManager || !mInputDeviceManager->Init())
    {
        Deinit();
        return false;
    }

    #ifdef WIN32
    mKeyboard = new Windows::WinKeyboard;
    if(!mKeyboard || !mKeyboard->Init())
    {
        Deinit();
        return false;
    }
    mInputDeviceManager->AddInputDevice(mKeyboard);
    #endif

    return true;
}

const bool InputImpl::Deinit()
{
    bool failed = false;

    if(mInputDeviceManager && !mInputDeviceManager->Deinit())
    {
        failed = true;
    }
    delete mInputDeviceManager;

    #ifdef WIN32
    if(mKeyboard && !mKeyboard->Deinit())
    {
        failed = true;
    }
    delete mKeyboard;
    #endif

    return !failed;
}

void InputImpl::Update(TimeType deltaT)
{
    if(mInputDeviceManager)
    {
        mInputDeviceManager->Update(deltaT);
    }
}

} // namespace jar
