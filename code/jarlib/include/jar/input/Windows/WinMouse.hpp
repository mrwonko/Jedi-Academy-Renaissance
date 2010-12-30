#ifndef JAR_INPUT_WINDOWS_WINMOUSE_HPP
#define JAR_INPUT_WINDOWS_WINMOUSE_HPP

#include "jar/input/InputDeviceMouse.hpp"
#include <windows.h>

namespace jar {
namespace Windows {

class WinMouse : public jar::InputDeviceMouse
{
    public:
        WinMouse();
        virtual ~WinMouse();

        const bool Init();

        virtual const bool Deinit();

        void InputHook(int nCode, WPARAM wParam, LPARAM lParam);
        HHOOK mInputHook;
    protected:
    private:
};

} // namespace Windows
} // namespace jar

#endif // JAR_INPUT_WINDOWS_WINMOUSE_HPP
