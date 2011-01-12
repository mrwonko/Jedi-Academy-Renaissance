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

#include "jar/input/InputDeviceJoystick.hpp"
#include "jar/input/Event.hpp"
#include "jar/core/Logger.hpp"

namespace jar {

InputDeviceJoystick::InputDeviceJoystick() :
    Index(0),
    mRumbleEnabled(true),
    mNumRumblers(0)
{
    //ctor
}

InputDeviceJoystick::~InputDeviceJoystick()
{
    //dtor
}

void InputDeviceJoystick::SetRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceJoystick::SetRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] = value;
}

void InputDeviceJoystick::IncreaseRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceJoystick::IncreaseRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] += value;
}

void InputDeviceJoystick::DecreaseRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceJoystick::DecreaseRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] -= value;
}

void InputDeviceJoystick::CheckRumbleStrengths()
{
    while(mRumbleStrengths.size() < mNumRumblers)
    {
        mRumbleStrengths.push_back(0.0f);
    }
    while(mRumbleStrengths.size() > mNumRumblers)
    {
        mRumbleStrengths.pop_back();
    }
}

void InputDeviceJoystick::JoyButtonPressed(unsigned int button)
{
    Event e;
    e.Type = Event::JoyButtonPressed;
    e.JoyButton.Button = button;
    e.JoyButton.JoyIndex = Index;
    SendEvent(e);
}

void InputDeviceJoystick::JoyButtonReleased(unsigned int button)
{
    Event e;
    e.Type = Event::JoyButtonReleased;
    e.JoyButton.Button = button;
    e.JoyButton.JoyIndex = Index;
    SendEvent(e);
}

void InputDeviceJoystick::JoyAxisMoved(unsigned int axis, float position)
{
    Event e;
    e.Type = Event::JoyAxisMoved;
    e.JoyAxis.Axis = axis;
    e.JoyAxis.Position = position;
    e.JoyButton.JoyIndex = Index;
    SendEvent(e);
}

} // namespace jar
