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
#include "jar/input/EventManager.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp>

//there's a damn #define N somewhere in boost! wth?
#ifdef N
#define N_OMG_WHATS_THAT N
#undef N
#endif

namespace jar
{

void Event::Luabind(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<jar::Key>("Key")
            .enum_("Code")
            [
                luabind::value("A", jar::Key::A),
                luabind::value("B", jar::Key::B),
                luabind::value("C", jar::Key::C),
                luabind::value("D", jar::Key::D),
                luabind::value("E", jar::Key::E),
                luabind::value("F", jar::Key::F),
                luabind::value("G", jar::Key::G),
                luabind::value("H", jar::Key::H),
                luabind::value("I", jar::Key::I),
                luabind::value("J", jar::Key::J),
                luabind::value("K", jar::Key::K),
                luabind::value("L", jar::Key::L),
                luabind::value("M", jar::Key::M),
                luabind::value("N", jar::Key::N),
                luabind::value("O", jar::Key::O),
                luabind::value("P", jar::Key::P),
                luabind::value("Q", jar::Key::Q),
                luabind::value("R", jar::Key::R),
                luabind::value("S", jar::Key::S),
                luabind::value("T", jar::Key::T),
                luabind::value("U", jar::Key::U),
                luabind::value("V", jar::Key::V),
                luabind::value("W", jar::Key::W),
                luabind::value("X", jar::Key::X),
                luabind::value("Y", jar::Key::Y),
                luabind::value("Z", jar::Key::Z),
                luabind::value("Num0", jar::Key::Num0),
                luabind::value("Num1", jar::Key::Num1),
                luabind::value("Num2", jar::Key::Num2),
                luabind::value("Num3", jar::Key::Num3),
                luabind::value("Num4", jar::Key::Num4),
                luabind::value("Num5", jar::Key::Num5),
                luabind::value("Num6", jar::Key::Num6),
                luabind::value("Num7", jar::Key::Num7),
                luabind::value("Num8", jar::Key::Num8),
                luabind::value("Num9", jar::Key::Num9),
                luabind::value("Escape",    jar::Key::Escape),
                luabind::value("LControl",  jar::Key::LControl),
                luabind::value("LShift",    jar::Key::LShift),
                luabind::value("LAlt",      jar::Key::LAlt),
                luabind::value("LSystem",   jar::Key::LSystem),
                luabind::value("RControl",  jar::Key::RControl),
                luabind::value("RShift",    jar::Key::RShift),
                luabind::value("RAlt",      jar::Key::RAlt),
                luabind::value("RSystem",   jar::Key::RSystem),
                luabind::value("Menu",      jar::Key::Menu),
                luabind::value("LBracket",  jar::Key::LBracket),
                luabind::value("RBracket",  jar::Key::RBracket),
                luabind::value("SemiColon", jar::Key::SemiColon),
                luabind::value("Comma",     jar::Key::Comma),
                luabind::value("Period",    jar::Key::Period),
                luabind::value("Quote",     jar::Key::Quote),
                luabind::value("Slash",     jar::Key::Slash),
                luabind::value("BackSlash", jar::Key::BackSlash),
                luabind::value("Tilde",     jar::Key::Tilde),
                luabind::value("Equal",     jar::Key::Equal),
                luabind::value("Dash",      jar::Key::Dash),
                luabind::value("Space",     jar::Key::Space),
                luabind::value("Return",    jar::Key::Return),
                luabind::value("Back",      jar::Key::Back),
                luabind::value("Tab",       jar::Key::Tab),
                luabind::value("PageUp",    jar::Key::PageUp),
                luabind::value("PageDown",  jar::Key::PageDown),
                luabind::value("End",       jar::Key::End),
                luabind::value("Home",      jar::Key::Home),
                luabind::value("Insert",    jar::Key::Insert),
                luabind::value("Delete",    jar::Key::Delete),
                luabind::value("Add",       jar::Key::Add),
                luabind::value("Subtract",  jar::Key::Subtract),
                luabind::value("Multiply",  jar::Key::Multiply),
                luabind::value("Divide",    jar::Key::Divide),
                luabind::value("Left",      jar::Key::Left),
                luabind::value("Right",     jar::Key::Right),
                luabind::value("Up",        jar::Key::Up),
                luabind::value("Down",      jar::Key::Down),
                luabind::value("Numpad0",   jar::Key::Numpad0),
                luabind::value("Numpad1",   jar::Key::Numpad1),
                luabind::value("Numpad2",   jar::Key::Numpad2),
                luabind::value("Numpad3",   jar::Key::Numpad3),
                luabind::value("Numpad4",   jar::Key::Numpad4),
                luabind::value("Numpad5",   jar::Key::Numpad5),
                luabind::value("Numpad6",   jar::Key::Numpad6),
                luabind::value("Numpad7",   jar::Key::Numpad7),
                luabind::value("Numpad8",   jar::Key::Numpad8),
                luabind::value("Numpad9",   jar::Key::Numpad9),
                luabind::value("F1", jar::Key::F1),
                luabind::value("F2", jar::Key::F2),
                luabind::value("F3", jar::Key::F3),
                luabind::value("F4", jar::Key::F4),
                luabind::value("F5", jar::Key::F5),
                luabind::value("F6", jar::Key::F6),
                luabind::value("F7", jar::Key::F7),
                luabind::value("F8", jar::Key::F8),
                luabind::value("F9", jar::Key::F9),
                luabind::value("F10", jar::Key::F10),
                luabind::value("F11", jar::Key::F11),
                luabind::value("F12", jar::Key::F12),
                luabind::value("F13", jar::Key::F13),
                luabind::value("F14", jar::Key::F14),
                luabind::value("F15", jar::Key::F15),
                luabind::value("Pause", jar::Key::Pause)

// commented out for now, since I'm using SFML Events internally and they don't have these.
//                luabind::value("CapsLock", jar::Key::CapsLock),
//                luabind::value("NumLock", jar::Key::NumLock),
//                luabind::value("ScrollLock", jar::Key::ScrollLock),
//                luabind::value("Print", jar::Key::Print),

//                luabind::value("Plus", jar::Key::Plus),
//                luabind::value("GER_LT", jar::Key::GER_LT)

            ],

        luabind::class_<jar::Event>("Event")
            .def(luabind::constructor<>())
            .def_readonly("Type", &Event::Type)
            //the event union - depends on Type.
            .def_readonly("JoyButton", &Event::JoyButton)
            .def_readonly("JoyAxis", &Event::JoyAxis)
            .def_readonly("Key", &Event::Key)
            .def_readonly("MouseMove", &Event::MouseMove)
            .def_readonly("MouseButton", &Event::MouseButton)
            .def_readonly("MouseWheel", &Event::MouseWheel)
            .scope
            [
                luabind::class_<Event::JoyAxisEvent>("JoyAxisEvent")
                    .def(luabind::constructor<>())
                    .def_readonly("JoyIndex", &Event::JoyAxisEvent::JoyIndex)
                    .def_readonly("Axis", &Event::JoyAxisEvent::Axis)
                    .def_readonly("Position", &Event::JoyAxisEvent::Position),

                luabind::class_<Event::JoyButtonEvent>("JoyButtonEvent")
                    .def(luabind::constructor<>())
                    .def_readonly("JoyIndex", &Event::JoyButtonEvent::JoyIndex)
                    .def_readonly("Button", &Event::JoyButtonEvent::Button),

                luabind::class_<Event::KeyEvent>("KeyEvent")
                    .def(luabind::constructor<>())
                    .def_readonly("Code", &Event::KeyEvent::Code),

                luabind::class_<Event::MouseButtonEvent>("MouseButtonEvent")
                    .def(luabind::constructor<>())
                    .def_readonly("Button", &Event::MouseButtonEvent::Button),

                luabind::class_<Event::MouseMovedEvent>("MouseMovedEvent")
                    .def(luabind::constructor<>())
                    .def_readonly("X", &Event::MouseMovedEvent::X)
                    .def_readonly("X", &Event::MouseMovedEvent::Y),

                luabind::class_<Event::MouseWheelEvent>("MouseWheelEvent")
                    .def(luabind::constructor<>())
                    .def_readonly("Delta", &Event::MouseWheelEvent::Delta)
            ]
            .enum_("EventType")
            [
                luabind::value("Closed", Event::Closed),
                luabind::value("KeyPressed", Event::KeyPressed),
                luabind::value("KeyReleased", Event::KeyReleased),
                luabind::value("MouseMoved", Event::MouseMoved),
                luabind::value("MouseButtonPressed", Event::MouseButtonPressed),
                luabind::value("MouseButtonReleased", Event::MouseButtonReleased),
                luabind::value("MouseWheelMoved", Event::MouseWheelMoved),
                luabind::value("JoyAxisMoved", Event::JoyAxisMoved),
                luabind::value("JoyButtonPressed", Event::JoyButtonPressed),
                luabind::value("JoyButtonReleased", Event::JoyButtonReleased)
            ],

        //doesn't really belong here, but I don't feel so little code justified a new file or even function
        luabind::class_<EventManager>("EventManager")
            .scope
            [
                luabind::def("GetSingleton", &EventManager::GetSingleton)
            ]
            .def("GetEvent", &EventManager::GetEvent, luabind::pure_out_value(_2))

    ];
}

} //namespace jar

#ifdef N_OMG_WHATS_THAT
#define N N_OMG_WHATS_THAT
#undef N_OMG_WHATS_THAT
#endif
