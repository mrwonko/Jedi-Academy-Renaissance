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

#include "jar/input/InputDeviceMouse.hpp"
#include "jar/input/Event.hpp"

namespace jar
{

InputDeviceMouse::InputDeviceMouse()
{
    //ctor
    mName = "Mouse";
}

InputDeviceMouse::~InputDeviceMouse()
{
    //dtor
}

void InputDeviceMouse::MouseButtonPressed(unsigned int button)
{
    Event e;
    e.Type = Event::MouseButtonPressed;
    e.MouseButton.Button = button;
    SendEvent(e);
}

void InputDeviceMouse::MouseButtonReleased(unsigned int button)
{
    Event e;
    e.Type = Event::MouseButtonReleased;
    e.MouseButton.Button = button;
    SendEvent(e);
}

void InputDeviceMouse::MouseMoved(int x, int y)
{
    Event e;
    e.Type = Event::MouseMoved;
    e.MouseMove.X = x;
    e.MouseMove.Y = y;
    SendEvent(e);
}

} // namespace jar
