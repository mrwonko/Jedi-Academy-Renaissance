#include "jar/input/InputImpl.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/Input.hpp"
#include <luabind/luabind.hpp>

namespace jar
{
    void InputImpl::Luabind(lua_State* state)
    {
        luabind::module(state, "jar")
        [
            luabind::class_<Input>("Input")
                .def("GetSingleton", &Input::GetSingleton)
                .def("GetInputDeviceManager", &Input::GetInputDeviceManager)
        ];
    }
}
