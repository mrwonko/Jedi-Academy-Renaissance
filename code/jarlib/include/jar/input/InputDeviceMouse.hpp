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

#ifndef JAR_INPUT_INPUTDEVICEMOUSE_HPP
#define JAR_INPUT_INPUTDEVICEMOUSE_HPP

#include "jar/input/InputDevice.hpp"
#include "jar/input/API.hpp"

namespace jar
{

class JARINPUTAPI InputDeviceMouse : public InputDevice
{
    public:
        /** destructor **/
        virtual ~InputDeviceMouse();

        virtual const DeviceType GetDeviceType() const { return DT_Mouse; }

    protected:
        /** constructor **/
        InputDeviceMouse();

        /** \brief Sends a MouseButtonPressed Event.
            \param button index of the pressed button **/
        void MouseButtonPressed(unsigned int button) const;

        /** \brief Sends a MouseButtonReleased Event.
            \param button index of the released button **/
        void MouseButtonReleased(unsigned int button) const;

        /** \brief Sends a MouseMoved Event. **/
        void MouseMoved(int x, int y) const;

        /** \brief Sends a MouseWheelMoved Event. **/
        void MouseWheelMoved(const int delta) const;
};

} // namespace jar

#endif // JAR_INPUTDEVICEMOUSE_HPP
