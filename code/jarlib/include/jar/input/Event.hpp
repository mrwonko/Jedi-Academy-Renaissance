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

#ifndef JAR_INPUT_EVENT_HPP
#define JAR_INPUT_EVENT_HPP

#include "jar/core/Time.hpp"
#include "jar/input/KeyCodes.hpp"
#include "jar/input/API.hpp"

class lua_State;

namespace sf
{
    class Event;
}

namespace jar
{

/**
    \brief Events that happen.

    Events include such things as key presses, mouse moves and the window closing.
**/
struct JARINPUTAPI Event
{
    /** \brief Create an event based on an SFML Event
        \return success
    **/
    const bool FromSFML(const sf::Event& e);

    /// Different types of events
    enum EventType
    {
        Closed, ///< The window close button has been clicked
        KeyPressed,
        KeyReleased,
        MouseMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseWheelMoved,
        JoyAxisMoved,
        JoyButtonPressed,
        JoyButtonReleased,
        //Resized, ///< The window has been resized
        LostFocus, ///< The window lost focus
        GainedFocus, ///< The window gained focus
    };
    ///the type of this event
    EventType Type;

    /** \brief Info for Event of EvenType KeyPressed or KeyReleased
    **/
    struct JARINPUTAPI KeyEvent
    {
        Key::Code Code;
    };

    /** \brief Info for Event of EventType MouseMoved
    **/
    struct JARINPUTAPI MouseMovedEvent
    {
        int X; ///< X-Position, from left side
        int Y; ///< Y-Position, from top
    };

    /** \brief Info for Event of EventType MouseWheelMoved
    **/
    struct JARINPUTAPI MouseWheelEvent
    {
        int Delta;
    };

    /** \brief Info for Event of EventType MouseButtonPressed or MouseButtonReleased
    **/
    struct JARINPUTAPI MouseButtonEvent
    {
        unsigned int Button; ///< The button index, 0 = LMB, 1 = RMB, 2 = MMB etc.
    };

    /** \brief Info for Event of EventType JoyButtonPressed or JoyButtonReleased
    **/
    struct JARINPUTAPI JoyButtonEvent
    {
        unsigned int JoyIndex; ///< The joystick index
        unsigned int Button; ///< the button index
    };

    /** \brief Info for Event of EventType JoyAxisMoved
    **/
    struct JARINPUTAPI JoyAxisEvent
    {
        unsigned int JoyIndex; ///< The joystick index
        unsigned int Axis; ///< The axis index
        float Position; ///< The Axis's position, in [-1; 1]
    };

    /// Information related to the event, depends on EventType.
    union
    {
        KeyEvent Key;
        MouseMovedEvent MouseMove;
        MouseButtonEvent MouseButton;
        MouseWheelEvent MouseWheel;
        JoyButtonEvent JoyButton;
        JoyAxisEvent JoyAxis;
    };

    /** \brief Binds Events to Lua via luabind **/
    static void Luabind(lua_State* L);
};

} // namespace jar

#endif // JAR_INPUT_EVENT_HPP
