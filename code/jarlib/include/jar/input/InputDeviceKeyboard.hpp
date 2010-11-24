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
