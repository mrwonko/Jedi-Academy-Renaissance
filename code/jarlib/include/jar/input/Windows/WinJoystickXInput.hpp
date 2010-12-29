#ifndef JAR_INPUT_WINDOWS_WINJOYSTICKXINPUT_H
#define JAR_INPUT_WINDOWS_WINJOYSTICKXINPUT_H

#define WINVER 0x0500 //I need some functions only available from windows 0x0500 onwards
#ifndef  WIN32_LEAN_AND_MEAN      // This cleans out rarely used stuff
#define  WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//these are used in xinput.h but they seem to be only of clarifying nature, still prevent gcc from compiling...
#define __in
#define __out
#define __reserved
#include <XInput.h>
#undef __in
#undef __out
#undef __reserved

#include <map>

#include "jar/input/InputDeviceJoystick.hpp"

namespace jar {
namespace Windows {


/**
    \brief Class for Gamepads (etc.) using XInput, i.e. (above all) the XBox 360 Gamepad.

    I need XInput because using DirectInput I don't get separate values for the two triggers of the 360 gamepad.

    \todo Test this!
**/
class WinJoystickXInput : public InputDeviceJoystick
{
    public:
        WinJoystickXInput();
        virtual ~WinJoystickXInput();

        /** \brief Initializes the joystick
            \param index the index of the joystick to initialize (usually in range [0, 3])
            \return true
        **/
        const bool Init(unsigned int index);

        virtual const bool Deinit();

        virtual void Update(TimeType deltaT);

    private:
        /// XInput index of this Gamepad
        DWORD mXIndex;
        XINPUT_STATE mLastState;

        void CheckAxis(int oldValue, int newValue, int multiplicator, unsigned int axis);

        std::map<unsigned int, unsigned int> mButtonMap;
};

} // namespace Windows
} // namespace jar

#endif // JAR_INPUT_WINDOWS_WINJOYSTICKXINPUT_H
