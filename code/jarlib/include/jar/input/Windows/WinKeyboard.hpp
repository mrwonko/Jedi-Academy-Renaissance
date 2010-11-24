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

#ifndef JAR_INPUT_WINDOWS_WINKEYBOARD_HPP
#define JAR_INPUT_WINDOWS_WINKEYBOARD_HPP

#define WINVER 0x0500 //I need some functions only available from windows 0x0500 onwards
#ifndef  WIN32_LEAN_AND_MEAN      // This cleans out rarely used stuff
#define  WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "jar/input/InputDeviceKeyboard.hpp"
#include <map>

namespace jar
{
namespace Windows
{
/** \brief Keyboard for Windows, using Hooking
    \bug RAlt also sends LControl, at least on my german keyboard
    \bug LALT behaves as if expecting a menu key, only key down event gets send, key up not until later and it blocks some other KeyDown-Events in the meantime.
    \bug It's partially localized - Y and Z may be switched, but special characters like &ouml; show up as Tile etc. Also the Numpad is translated into Num0/End, Num1/Down etc.
    \todo Use <a href=http://msdn.microsoft.com/en-us/library/ms644985%28v=VS.85%29.aspx>LowLevelKeyboardProc</a> instead? May fix the bugs...
**/
class WinKeyboard : public InputDeviceKeyboard
{
    public:
        WinKeyboard();
        virtual ~WinKeyboard();

        /** \brief Initializes the Keyboard **/
        const bool Init();

        /** \brief Deinitializes the Keyboard **/
        const bool Deinit();

        void InputHook(int nCode, WPARAM wParam, LPARAM lParam);
        HHOOK mInputHook;
    private:
        void KeyEvent(const bool released, Key::Code code);
        std::map<WPARAM, jar::Key::Code> mKeyMap;
};

} // namespace Windows
} // namespace jar

#endif // JAR_INPUT_WINDOWS_WINKEYBOARD_HPP
