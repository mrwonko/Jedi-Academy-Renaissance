#ifndef JAR_INPUTDEVICEKEYBOARD_HPP
#define JAR_INPUTDEVICEKEYBOARD_HPP

#include "jar/InputDevice.hpp"
#include "jar/KeyCodes.hpp"

namespace jar
{

class InputDeviceKeyboard : public InputDevice
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

#endif // JAR_INPUTDEVICEKEYBOARD_HPP
