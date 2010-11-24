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

#include "jar/input/InputDeviceKeyboard.hpp"
#include "jar/input/Event.hpp"

namespace jar
{

InputDeviceKeyboard::InputDeviceKeyboard()
{
    //ctor
    mName = "Keyboard";
}

InputDeviceKeyboard::~InputDeviceKeyboard()
{
    //dtor
}

void InputDeviceKeyboard::KeyPressed(Key::Code code)
{
    Event e;
    e.Type = Event::KeyPressed;
    e.Key.Code = code;
    SendEvent(e);
}

void InputDeviceKeyboard::KeyReleased(Key::Code code)
{
    Event e;
    e.Type = Event::KeyReleased;
    e.Key.Code = code;
    SendEvent(e);
}

} // namespace jar
