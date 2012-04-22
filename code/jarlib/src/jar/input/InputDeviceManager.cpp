#include "jar/input/InputDeviceManager.hpp"
#include "jar/input/InputDevice.hpp"
#include "jar/input/InputDeviceController.hpp"
#include "jar/input/EventManager.hpp"
#include "jar/Input.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"

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

const bool InputDeviceManager::Deinit()
{
    //nothing to do here yet
    return true;
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

InputDeviceController* InputDeviceManager::GetController(const unsigned int index) const
{
    for(std::set<InputDevice*>::const_iterator it = mDevices.begin(); it != mDevices.end(); ++it)
    {
        if((*it)->GetDeviceType() != InputDevice::DT_Controller) continue;
        //this device is a joystick. But is it the right one?
        InputDeviceController* joy = dynamic_cast<InputDeviceController*>(*it);
        if(joy->GetIndex() == index) return joy;
    }
    Logger::GetDefaultLogger().Warning("Input Device Manager: Request for invalid joystick #" + Helpers::IntToString(index));
    return NULL;
}

std::list<InputDeviceController*>& InputDeviceManager::GetAllControllers() const
{
    static std::list<InputDeviceController*> l;
    l.clear();
    for(std::set<InputDevice*>::const_iterator it = mDevices.begin(); it != mDevices.end(); ++it)
    {
        //skip if no joystick
        if((*it)->GetDeviceType() != InputDevice::DT_Controller) continue;
        l.push_back(dynamic_cast<InputDeviceController*>(*it));
    }
    return l;
}

void InputDeviceManager::ReceiveEvent(const Event& event)
{
    //the EventManager takes care of events
    assert(EventManager::HasSingleton());
    EventManager::GetSingleton().ReceiveEvent(event);
}

} // namespace jar
