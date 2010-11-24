#ifndef JAR_INPUT_INPUTDEVICEJOYSTICK_HPP
#define JAR_INPUT_INPUTDEVICEJOYSTICK_HPP

#include "jar/input/InputDevice.hpp"
#include "jar/input/API.hpp"

namespace jar {

class JARINPUTAPI InputDeviceJoystick : public InputDevice
{
    public:
        /** \brief Destructor **/
        virtual ~InputDeviceJoystick();

        virtual const DeviceType GetDeviceType() { return DT_Joystick; }

        unsigned int Index;

    protected:
        /** \brief Constructor **/
        InputDeviceJoystick();

        /** \brief Sends a JoyButtonPressed Event.
            \param button index of the pressed button **/
        void JoyButtonPressed(unsigned int button);

        /** \brief Sends a MouseButtonReleased Event.
            \param button index of the released button **/
        void JoyButtonReleased(unsigned int button);

        /** \brief Sends a JoyAxisMoved Event. **/
        void JoyAxisMoved(unsigned int axis, float position);

};

} // namespace jar

#endif // JAR_INPUTDEVICEJOYSTICK_HPP
