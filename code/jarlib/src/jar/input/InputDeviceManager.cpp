/*
===========================================================================
Copyright (C) 2010 Willi Schinmeyer

This file is part of the Jedi Academy: Renaissance source code.

Jedi Academy: Renaissance source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Jedi Academy: Renaissance source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jedi Academy: Renaissance source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "jar/input/InputDeviceManager.hpp"
#include "jar/input/InputDevice.hpp"
#include "jar/input/InputDeviceJoystick.hpp"
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

InputDeviceJoystick* InputDeviceManager::GetJoystick(const unsigned int index) const
{
    for(std::set<InputDevice*>::const_iterator it = mDevices.begin(); it != mDevices.end(); ++it)
    {
        if((*it)->GetDeviceType() != InputDevice::DT_Joystick) continue;
        //this device is a joystick. But is it the right one?
        InputDeviceJoystick* joy = dynamic_cast<InputDeviceJoystick*>(*it);
        if(joy->GetIndex() == index) return joy;
    }
    Logger::GetDefaultLogger().Warning("Input Device Manager: Request for invalid joystick #" + Helpers::IntToString(index));
    return NULL;
}

std::list<InputDeviceJoystick*>& InputDeviceManager::GetAllJoysticks() const
{
    static std::list<InputDeviceJoystick*> l;
    l.clear();
    for(std::set<InputDevice*>::const_iterator it = mDevices.begin(); it != mDevices.end(); ++it)
    {
        //skip if no joystick
        if((*it)->GetDeviceType() != InputDevice::DT_Joystick) continue;
        l.push_back(dynamic_cast<InputDeviceJoystick*>(*it));
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
