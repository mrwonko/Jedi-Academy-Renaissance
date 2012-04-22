#ifndef JAR_INPUT_WINDOWS_WINJOYSTICKXINPUT_H
#define JAR_INPUT_WINDOWS_WINJOYSTICKXINPUT_H

//I need some functions only available from windows 0x0500 onwards, thus WINVER is set to 0x0500 in the build options (not here, got warning about redifinition)
#ifndef  WIN32_LEAN_AND_MEAN      // This cleans out rarely used stuff
#define  WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

//these are used in xinput.h but they seem to be only of clarifying nature, still prevent gcc from compiling...
#ifdef __GNUC__
#define __in
#define __out
#define __reserved
#endif

#include <XInput.h>

#ifdef __GNUC__
#undef __in
#undef __out
#undef __reserved
#endif

#include <map>

#include "jar/input/InputDeviceController.hpp"

namespace jar {
namespace Windows {


/**
    \brief Class for Gamepads (etc.) using XInput, i.e. (above all) the XBox 360 Gamepad.

    I need XInput because using DirectInput I don't get separate values for the two triggers of the 360 gamepad.

    \todo Test this!
**/
class WinControllerXInput : public InputDeviceController, public Updatable
{
    public:
        WinControllerXInput(unsigned int index);
        virtual ~WinControllerXInput();

        /** \brief Initializes the joystick
            \param index the index of the joystick to initialize (usually in range [0, 3])
            \return true
        **/
        const bool Init(unsigned int index);

        virtual const bool Deinit();

        virtual void Update(const TimeType deltaT);

        virtual std::string GetUniqueID() const;

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
