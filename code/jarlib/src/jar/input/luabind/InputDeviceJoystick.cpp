#include "jar/input/InputDeviceJoystick.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace jar
{
    void InputDeviceJoystick::Luabind(lua_State* L)
    {
        luabind::module(L, "jar")
        [
            luabind::class_<InputDeviceJoystick>("InputDeviceJoystick")
                .def("GetIndex", &InputDeviceJoystick::GetIndex)
                .def("GetUniqueID", &InputDeviceJoystick::GetUniqueID)
                .def("GetName", &InputDevice::GetName)
        ];
    }
}
