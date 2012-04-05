#ifndef JAR_INPUT_INPUTDEVICE_HPP
#define JAR_INPUT_INPUTDEVICE_HPP

#include "jar/core/Time.hpp"
#include "jar/input/EventSender.hpp"
#include "jar/input/API.hpp"
#include "jar/core/Updatable.hpp"

#include <string>

namespace jar
{

/** \brief An input devices

    Input devices send their input via events, i.e. SendEvent().
**/
class JARINPUTAPI InputDevice : public EventSender
{
    public:
        virtual ~InputDevice();

        /** \brief Deinitializes the device

            Overwrite this when inheriting, if necessary.
        **/
        virtual const bool Deinit() {return true;}

        /** \brief Returns the name of this device
            \return device name
        **/
        const std::string& GetName() { return mName; }

        /// of what Type a Device is
        enum DeviceType
        {
            DT_Keyboard,
            DT_Mouse,
            DT_Controller, ///< Joystick, Gamepad, Steeringwheel etc.
        };

        /** \brief Gives this device's type
            \return The DeviceType
        **/
        virtual const DeviceType GetDeviceType() const = 0;

    protected:
        InputDevice();
        std::string mName;
    private:
};

} // namespace jar

#endif // JAR_INPUT_INPUTDEVICE_HPP
