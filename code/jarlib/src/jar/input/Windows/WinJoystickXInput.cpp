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

#include "jar/input/Windows/WinJoystickXInput.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/Input.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"
#include <algorithm>

namespace jar {
namespace Windows {

WinJoystickXInput::WinJoystickXInput()
{
    //ctor
    mButtonMap[XINPUT_GAMEPAD_DPAD_UP] = 0;
    mButtonMap[XINPUT_GAMEPAD_DPAD_DOWN] = 1;
    mButtonMap[XINPUT_GAMEPAD_DPAD_LEFT] = 2;
    mButtonMap[XINPUT_GAMEPAD_DPAD_RIGHT] = 3;

    // 0 - 15 reserved for DPads

    mButtonMap[XINPUT_GAMEPAD_A] = 16;
    mButtonMap[XINPUT_GAMEPAD_B] = 17;
    mButtonMap[XINPUT_GAMEPAD_X] = 18;
    mButtonMap[XINPUT_GAMEPAD_Y] = 19;
    mButtonMap[XINPUT_GAMEPAD_LEFT_SHOULDER] = 20;
    mButtonMap[XINPUT_GAMEPAD_RIGHT_SHOULDER] = 21;
    mButtonMap[XINPUT_GAMEPAD_BACK] = 22;
    mButtonMap[XINPUT_GAMEPAD_START] = 23;
    mButtonMap[XINPUT_GAMEPAD_LEFT_THUMB] = 24;
    mButtonMap[XINPUT_GAMEPAD_RIGHT_THUMB] = 25;

    mNumRumblers = 2;
    CheckRumbleStrengths();
}

WinJoystickXInput::~WinJoystickXInput()
{
    //dtor
}

const bool WinJoystickXInput::Init(unsigned int index)
{
    mXIndex = index;
    if(XInputGetState(mXIndex, &mLastState) != ERROR_SUCCESS)
    {
        return false;
    }
    mName = "XInput Controller " + Helpers::IntToString(index);

    if(XInputGetState(mXIndex, &mLastState) != ERROR_SUCCESS)
    {
        //could not update? That's bad.
        Logger::GetDefaultLogger().Error("Could not do test query on XInput Gamepad " + Helpers::IntToString(mXIndex) + ", ignoring it.");
        Deinit();
        return false;
    }

    return true;
}

const bool WinJoystickXInput::Deinit()
{
    //not much to do here :-)
    return true;
}

void WinJoystickXInput::Update(TimeType deltaT)
{
    //  read state

    XINPUT_STATE state;
    //try to update the state
    if(XInputGetState(mXIndex, &state) != ERROR_SUCCESS)
    {
        //could not update? That's bad.
        Logger::GetDefaultLogger().Warning("Could not update XInput Gamepad " + Helpers::IntToString(mXIndex) + ", removing.");
        Input::GetSingleton().GetInputDeviceManager().RemoveInputDevice(this); //Is this safe? It seems to be.
        //TODO: unpress any pressed buttons, reset axes
        return;
    }

    if(state.dwPacketNumber == mLastState.dwPacketNumber) //nothing changed
    {
        return;
    }

    //check for button presses
    for(std::map<unsigned int, unsigned int>::iterator it = mButtonMap.begin(); it != mButtonMap.end(); ++it)
    {
        if( (state.Gamepad.wButtons & it->first) != (mLastState.Gamepad.wButtons & it->first) )
        {
            if(state.Gamepad.wButtons & it->first)
            {
                JoyButtonPressed(it->second);
            }
            else
            {
                JoyButtonReleased(it->second);
            }
        }
    }
    //check for axis movements
    CheckAxis(mLastState.Gamepad.sThumbLX, state.Gamepad.sThumbLX, 32768, 0);
    CheckAxis(mLastState.Gamepad.sThumbLY, state.Gamepad.sThumbLY, 32768, 1);
    CheckAxis(mLastState.Gamepad.sThumbRX, state.Gamepad.sThumbRX, 32768, 2);
    CheckAxis(mLastState.Gamepad.sThumbRY, state.Gamepad.sThumbRY, 32768, 3);
    CheckAxis(mLastState.Gamepad.bLeftTrigger, state.Gamepad.bLeftTrigger, 255, 4);
    CheckAxis(mLastState.Gamepad.bRightTrigger, state.Gamepad.bRightTrigger, 255, 5);

    mLastState = state;

    //  set rumbling

    //clamp rumble amount to [0, 1]
    float left = std::max(0.0f, std::min(1.0f, mRumbleStrengths[0]));
    float right = std::max(0.0f, std::min(1.0f, mRumbleStrengths[1]));

    XINPUT_VIBRATION vib;
    vib.wLeftMotorSpeed = WORD(65535 * left);
    vib.wRightMotorSpeed = WORD(65535 * right);
    XInputSetState(mXIndex, &vib);
}

void WinJoystickXInput::CheckAxis(int oldValue, int newValue, int multiplicator, unsigned int axis)
{
    if(oldValue != newValue)
    {
        //the vertical axes are inverted from DirectInput, let's undo that.
        if(axis == 1 || axis == 3)
        {
            JoyAxisMoved(axis, -((float)newValue)/multiplicator);
        }
        else
        {
            JoyAxisMoved(axis, ((float)newValue)/multiplicator);
        }
    }
}

} // namespace Windows
} // namespace jar
