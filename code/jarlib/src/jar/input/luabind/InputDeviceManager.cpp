#include "jar/input/InputDeviceManager.hpp"
#include "jar/input/InputDeviceJoystick.hpp"
#include <luabind/luabind.hpp>
#include <luabind/iterator_policy.hpp>

namespace jar
{
    void InputDeviceManager::Luabind(lua_State* state)
    {
        luabind::module(state, "jar")
        [
            luabind::class_<InputDeviceManager>("InputDeviceManager")
                .def("GetAllJoysticks", &InputDeviceManager::GetAllJoysticks, luabind::return_stl_iterator)
        ];
    }
}
