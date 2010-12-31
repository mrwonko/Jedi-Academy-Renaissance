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

#include "jar/input/Event.hpp"
#include "SFML/Window/Event.hpp"
#include <map>

namespace jar
{

const bool Event::FromSFML(const sf::Event& e)
{
    static std::map<sf::Key::Code, jar::Key::Code> keyMap;
    if(keyMap.size() == 0)
    {
        for(unsigned long i = sf::Key::A; i <= sf::Key::Z; ++i)
        {
            keyMap[sf::Key::Code(i)] = Key::Code(i - sf::Key::A + Key::A);
        }
        for(unsigned long i = sf::Key::Num0; i <= sf::Key::Num9; ++i)
        {
            keyMap[sf::Key::Code(i)] = Key::Code(i - sf::Key::Num0 + Key::Num0);
        }
        keyMap[sf::Key::Escape] = Key::Escape;
        keyMap[sf::Key::LControl] = Key::LControl;
        keyMap[sf::Key::LShift] = Key::LShift;
        keyMap[sf::Key::LAlt] = Key::LAlt;
        keyMap[sf::Key::LSystem] = Key::LSystem;
        keyMap[sf::Key::RControl] = Key::RControl;
        keyMap[sf::Key::RShift] = Key::RShift;
        keyMap[sf::Key::RAlt] = Key::RAlt;
        keyMap[sf::Key::RSystem] = Key::RSystem;
        keyMap[sf::Key::Menu] = Key::Menu;
        keyMap[sf::Key::LBracket] = Key::LBracket;
        keyMap[sf::Key::RBracket] = Key::RBracket;
        keyMap[sf::Key::SemiColon] = Key::SemiColon;
        keyMap[sf::Key::Comma] = Key::Comma;
        keyMap[sf::Key::Period] = Key::Period;
        keyMap[sf::Key::Quote] = Key::Quote;
        keyMap[sf::Key::Slash] = Key::Slash;
        keyMap[sf::Key::BackSlash] = Key::BackSlash;
        keyMap[sf::Key::Tilde] = Key::Tilde;
        keyMap[sf::Key::Equal] = Key::Equal;
        keyMap[sf::Key::Dash] = Key::Dash;
        keyMap[sf::Key::Space] = Key::Space;
        keyMap[sf::Key::Return] = Key::Return;
        keyMap[sf::Key::Back] = Key::Back;
        keyMap[sf::Key::Tab] = Key::Tab;
        keyMap[sf::Key::PageUp] = Key::PageUp;
        keyMap[sf::Key::PageDown] = Key::PageDown;
        keyMap[sf::Key::End] = Key::End;
        keyMap[sf::Key::Home] = Key::Home;
        keyMap[sf::Key::Insert] = Key::Insert;
        keyMap[sf::Key::Delete] = Key::Delete;
        keyMap[sf::Key::Add] = Key::Add;
        keyMap[sf::Key::Subtract] = Key::Subtract;
        keyMap[sf::Key::Multiply] = Key::Multiply;
        keyMap[sf::Key::Divide] = Key::Divide;
        keyMap[sf::Key::Left] = Key::Left;
        keyMap[sf::Key::Right] = Key::Right;
        keyMap[sf::Key::Up] = Key::Up;
        keyMap[sf::Key::Down] = Key::Down;
        keyMap[sf::Key::Numpad0] = Key::Numpad0;
        keyMap[sf::Key::Numpad1] = Key::Numpad1;
        keyMap[sf::Key::Numpad2] = Key::Numpad2;
        keyMap[sf::Key::Numpad3] = Key::Numpad3;
        keyMap[sf::Key::Numpad4] = Key::Numpad4;
        keyMap[sf::Key::Numpad5] = Key::Numpad5;
        keyMap[sf::Key::Numpad6] = Key::Numpad6;
        keyMap[sf::Key::Numpad7] = Key::Numpad7;
        keyMap[sf::Key::Numpad8] = Key::Numpad8;
        keyMap[sf::Key::Numpad9] = Key::Numpad9;
        keyMap[sf::Key::F1] = Key::F1;
        keyMap[sf::Key::F2] = Key::F2;
        keyMap[sf::Key::F3] = Key::F3;
        keyMap[sf::Key::F4] = Key::F4;
        keyMap[sf::Key::F5] = Key::F5;
        keyMap[sf::Key::F6] = Key::F6;
        keyMap[sf::Key::F7] = Key::F7;
        keyMap[sf::Key::F8] = Key::F8;
        keyMap[sf::Key::F9] = Key::F9;
        keyMap[sf::Key::F10] = Key::F10;
        keyMap[sf::Key::F11] = Key::F11;
        keyMap[sf::Key::F12] = Key::F12;
        keyMap[sf::Key::F13] = Key::F13;
        keyMap[sf::Key::F14] = Key::F14;
        keyMap[sf::Key::F15] = Key::F15;
        keyMap[sf::Key::Pause] = Key::Pause;
    }

    switch(e.Type)
    {
        case sf::Event::Closed:
        {
            Type = Event::Closed;
            return true;
        }
        case sf::Event::GainedFocus:
        {
            Type = Event::GainedFocus;
            return true;
        }
        case sf::Event::LostFocus:
        {
            Type = Event::LostFocus;
            return true;
        }
        case sf::Event::KeyPressed:
        {
            if(keyMap.find(e.Key.Code) != keyMap.end())
            {
                Type = Event::KeyPressed;
                Key.Code = keyMap[e.Key.Code];
                return true;
            }
            //else
            return false;
        }
        case sf::Event::KeyReleased:
        {
            if(keyMap.find(e.Key.Code) != keyMap.end())
            {
                Type = Event::KeyReleased;
                Key.Code = keyMap[e.Key.Code];
                return true;
            }
            //else
            return false;
        }
        case sf::Event::MouseMoved:
        {
            Type = Event::MouseMoved;
            MouseMove.X = e.MouseMove.X;
            MouseMove.Y = e.MouseMove.Y;
            return true;
        }
        case sf::Event::MouseButtonPressed:
        {
            Type = Event::MouseButtonPressed;
            MouseButton.Button = e.MouseButton.Button;
            return true;
        }
        case sf::Event::MouseButtonReleased:
        {
            Type = Event::MouseButtonReleased;
            MouseButton.Button = e.MouseButton.Button;
            return true;
        }
        case sf::Event::MouseWheelMoved:
        {
            Type = Event::MouseWheelMoved;
            MouseWheel.Delta = e.MouseWheel.Delta;
            return true;
        }
        #ifndef _WIN32 //joysticks for win32 handled separately via DirectInput & XInput
        case sf::Event::JoyButtonPressed:
        {
            Type = Event::JoyButtonPressed;
            JoyButton.Button = e.JoyButton.Button;
            JoyButton.JoyIndex = e.JoyButton.JoystickId;
            return true;
        }
        case sf::Event::JoyButtonReleased:
        {
            Type = Event::JoyButtonReleased;
            JoyButton.Button = e.JoyButton.Button;
            JoyButton.JoyIndex = e.JoyButton.JoystickId;
            return true;
        }
        case sf::Event::JoyMoved:
        {
            Type = Event::JoyAxisMoved;
            JoyAxis.Axis = e.JoyMove.Axis;
            JoyAxis.JoyIndex = e.JoyMove.JoystickId;
            JoyAxis.Position = e.JoyMove.Position;
            return true;
        }
        #endif
        default:
        {
            return false;
        }
    }
    return false;
}

}
