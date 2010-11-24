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

#ifndef JAR_INPUT_INPUTDEVICEKEYBOARD_HPP
#define JAR_INPUT_INPUTDEVICEKEYBOARD_HPP

#include "jar/input/InputDevice.hpp"
#include "jar/input/KeyCodes.hpp"
#include "jar/input/API.hpp"

namespace jar
{

class JARINPUTAPI InputDeviceKeyboard : public InputDevice
{
    public:
        /** destructor **/
        virtual ~InputDeviceKeyboard();

        virtual const DeviceType GetDeviceType() { return DT_Keyboard; }

    protected:
        /** constructor **/
        InputDeviceKeyboard();

        /** \brief Sends a KeyPressed Event.
            \param code \link Key::Code code \endlink of the pressed key. **/
        void KeyPressed(Key::Code code);

        /** \brief Sends a KeyReleased Event.
            \param code \link Key::Code code \endlink of the released key. **/
        void KeyReleased(Key::Code code);
};

} // namespace jar

#endif // JAR_INPUT_INPUTDEVICEKEYBOARD_HPP
