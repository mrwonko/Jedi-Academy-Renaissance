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

        virtual const DeviceType GetDeviceType() { return DT_Mouse; }

    protected:
        /** constructor **/
        InputDeviceMouse();

        /** \brief Sends a MouseButtonPressed Event.
            \param button index of the pressed button **/
        void MouseButtonPressed(unsigned int button);

        /** \brief Sends a MouseButtonReleased Event.
            \param button index of the released button **/
        void MouseButtonReleased(unsigned int button);

        /** \brief Sends a MouseMoved Event. **/
        void MouseMoved(int x, int y);
};

} // namespace jar

#endif // JAR_INPUTDEVICEMOUSE_HPP
