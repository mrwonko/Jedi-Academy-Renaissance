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

#ifndef JAR_INPUT_INPUTIMPL_HPP
#define JAR_INPUT_INPUTIMPL_HPP

#ifdef _WIN32

//I need some functions only available from windows 0x0500 onwards, thus WINVER is set to 0x0500 in the build options (not here, got warning about redifinition)
#ifndef  WIN32_LEAN_AND_MEAN  // This cleans out rarely used stuff
#define  WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//if you change this, also change it in WinJoystickDirectInput.hpp
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#endif

#include "jar/core/Component.hpp"
#include <vector>
#include <cassert>

class lua_State;

namespace jar {

class InputDeviceManager;
class EventManager;

#ifdef _WIN32
namespace Windows
{
    class WinKeyboard;
    class WinMouse;
    class WinJoystickXInput;
    class WinJoystickDirectInput;
}
#endif

/**
    \bug On Windows input will only work for one window - there's no telling which.
**/

class InputImpl : public Component
{
    public:
        InputImpl();
        virtual ~InputImpl();

        virtual const bool Init();
        const bool InitJoysticks();

        virtual const bool Deinit();
        const bool DeinitJoysticks();

        InputDeviceManager& GetInputDeviceManager() { assert(mInputDeviceManager); return *mInputDeviceManager; }
        EventManager& GetEventManager() { assert(mEventManager); return *mEventManager; }

        void OnWindowCreated();

        /** \brief Exposes Input to Lua (not InputImpl!) **/
        static void Luabind(lua_State* state);

    private:
        InputDeviceManager* mInputDeviceManager;
        EventManager* mEventManager;
#ifdef _WIN32
        //XInput
        std::vector<Windows::WinJoystickXInput*> mXInputJoysticks;
        //DirectInput
        LPDIRECTINPUT8 mDirectInput;
        std::vector<Windows::WinJoystickDirectInput*> mDirectInputJoysticks;

        bool mNoWindowExists;
#else
#warning input not yet supported on this OS!
#endif
};

} // namespace jar

#endif // JAR_INPUT_INPUTIMPL_HPP
