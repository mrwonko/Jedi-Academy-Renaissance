#include "jar/InputDeviceManager.hpp"
#include "jar/InputDevice.hpp"

namespace jar
{

InputDeviceManager::InputDeviceManager() :
    mInitialized(false)
{
    //ctor
}

InputDeviceManager::~InputDeviceManager()
{
    //dtor
    if(mInitialized)
    {
        Deinit();
    }
}

const bool InputDeviceManager::Init()
{
    mInitialized = true;
    return true;
}

void InputDeviceManager::Deinit()
{
    //nothing to do here yet
}

void InputDeviceManager::AddInputDevice(InputDevice* device)
{
    mDevices.insert(device);
    device->RegisterListener(this);
}

const bool InputDeviceManager::RemoveInputDevice(InputDevice* device)
{
    for(std::set<InputDevice*>::iterator it = mDevices.begin(); it != mDevices.end(); ++it)
    {
        if(*it == device)
        {
            mDevices.erase(it);
            return true;
        }
    }
    return false;
}

const bool InputDeviceManager::DeleteInputDevice(InputDevice* device)
{
    device->Deinit();
    const bool ret = RemoveInputDevice(device);
    delete device;
    return ret;
}

void InputDeviceManager::Update(TimeType deltaT)
{
    for(std::set<InputDevice*>::iterator it = mDevices.begin(); it != mDevices.end(); ++it)
    {
        (*it)->Update(deltaT);
    }
}

} // namespace jar
