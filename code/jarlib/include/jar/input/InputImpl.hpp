#ifndef JAR_INPUT_INPUTIMPL_HPP
#define JAR_INPUT_INPUTIMPL_HPP

#ifdef _WIN32

//I need some functions only available from windows 0x0500 onwards, thus WINVER is set to 0x0500 in the build options (not here, got warning about redifinition)
#ifndef  WIN32_LEAN_AND_MEAN  // This cleans out rarely used stuff
#define  WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//if you change this, also change it in WinControllerDirectInput.hpp
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#endif

#include "jar/core/Component.hpp"
#include <vector>
#include <cassert>

class lua_State;

namespace jar {

class InputDeviceManager;
class EventManager;

#ifdef _WIN32
namespace Windows
{
    class WinKeyboard;
    class WinMouse;
    class WinControllerXInput;
    class WinControllerDirectInput;
}
#endif

/**
    \bug On Windows input will only work for one window - there's no telling which.
**/

class InputImpl : public Component
{
    public:
        InputImpl();
        virtual ~InputImpl();

        virtual const bool Init();
        const bool InitJoysticks();

        virtual const bool Deinit();
        const bool DeinitJoysticks();

        InputDeviceManager& GetInputDeviceManager() { assert(mInputDeviceManager); return *mInputDeviceManager; }
        EventManager& GetEventManager() { assert(mEventManager); return *mEventManager; }

        void OnWindowCreated();

        /** \brief Exposes Input to Lua (not InputImpl!) **/
        static void Luabind(lua_State* state);

    private:
        InputDeviceManager* mInputDeviceManager;
        EventManager* mEventManager;
#ifdef _WIN32
        //XInput
        std::vector<Windows::WinControllerXInput*> mXInputJoysticks;
        //DirectInput
        LPDIRECTINPUT8 mDirectInput;
        std::vector<Windows::WinControllerDirectInput*> mDirectInputJoysticks;

        bool mNoWindowExists;
#else
#warning joystick input not yet supported on this OS!
#endif
};

} // namespace jar

#endif // JAR_INPUT_INPUTIMPL_HPP
