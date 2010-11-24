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

#ifndef JAR_INPUT_INPUTDEVICE_HPP
#define JAR_INPUT_INPUTDEVICE_HPP

#include "jar/core/Time.hpp"
#include "jar/input/EventSender.hpp"
#include "jar/input/API.hpp"

#include <string>

namespace jar
{

/** \brief An input devices

    Input devices send their input via events, i.e. SendEvent(). They are to include a timestamp.
**/
class JARINPUTAPI InputDevice : public EventSender
{
    public:
        virtual ~InputDevice();

        /** \brief Initializes the Device

            Overwrite this when inheriting, if necessary, usually returns true.
            \return success
        **/
        virtual const bool Init();

        /** \brief Deinitializes the device

            Overwrite this when inheriting, if necessary.
        **/
        virtual const bool Deinit() {return true;}

        /** \brief Called every frame with the time since the last frame

            Overwrite this when inheriting, if you need anything done each frame.
            \param deltaT time since last frame
        **/
        virtual void Update(TimeType deltaT);

        /** \brief Returns the name of this device
            \return device name
        **/
        const std::string& GetName() { return mName; }

        /// of what Type a Device is
        enum DeviceType
        {
            DT_Keyboard,
            DT_Mouse,
            DT_Joystick, ///< Joystick, Gamepad, Steeringwheel etc.
        };

        /** \brief Gives this device's type
            \return The DeviceType
        **/
        virtual const DeviceType GetDeviceType() = 0;

    protected:
        InputDevice();
        std::string mName;
    private:
};

} // namespace jar

#endif // JAR_INPUT_INPUTDEVICE_HPP
