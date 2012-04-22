#include "jar/input/InputDeviceController.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace jar
{
    void InputDeviceController::Luabind(lua_State* L)
    {
        luabind::module(L, "jar")
        [
            luabind::class_<InputDeviceController>("InputDeviceController")
                .def("GetIndex", &InputDeviceController::GetIndex)
                .def("GetUniqueID", &InputDeviceController::GetUniqueID)
                .def("GetName", &InputDevice::GetName)
        ];
    }
}
