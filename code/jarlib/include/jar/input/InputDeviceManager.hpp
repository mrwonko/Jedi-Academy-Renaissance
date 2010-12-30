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

#ifndef JAR_INPUT_INPUTDEVICEMANAGER_HPP
#define JAR_INPUT_INPUTDEVICEMANAGER_HPP

#include "jar/input/EventSender.hpp"
#include "jar/input/EventListener.hpp"
#include "jar/core/Time.hpp"
#include "jar/input/API.hpp"
#include <set>

namespace jar
{

class InputDevice;
class InputDeviceJoystick;
class JARINPUTAPI InputDeviceManager : public EventListener
{
    public:
        /** \brief Constructor **/
        InputDeviceManager();
        /** \brief Destructor **/
        ~InputDeviceManager();

        /** \brief Initializes the device manager. <b>Does not create any devices!</b>

            \return success
        **/
        const bool Init();

        /** \brief Deinitializes the device manager. If you don't call it, it's called in the destructor.
        **/
        const bool Deinit();

        /** \brief Adds an input device to the manager.

            \param device The device to be added

        **/
        void AddInputDevice(InputDevice* device);

        /** \brief Removes an input device from the manager. <b>Does not delete the device!</b>

            \param device The InputDevice
            \return Whether the device existed

        **/
        const bool RemoveInputDevice(InputDevice* device);

        /** \brief Deinitializes, deletes and removes an InputDevice from the manager.
            \param device The InputDevice
            \return Whether the device existed in the manager. Even if it didn't it still got deleted.
        **/
        const bool DeleteInputDevice(InputDevice* device);

        /** \brief Retrieve a joystick of a given index
            \return The joystick, or NULL if no such joystick exists. **/
        InputDeviceJoystick* GetJoystick(const unsigned int index) const;

        virtual void ReceiveEvent(const Event& event);
    private:
        std::set<InputDevice*> mDevices;
        bool mInitialized;
};

} // namespace jar

#endif // JAR_INPUT_INPUTDEVICEMANAGER_HPP
