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
