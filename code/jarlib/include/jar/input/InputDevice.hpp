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
