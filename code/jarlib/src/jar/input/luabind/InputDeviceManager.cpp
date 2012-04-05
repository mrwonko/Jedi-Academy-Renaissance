#include "jar/input/InputDeviceManager.hpp"
#include "jar/input/InputDeviceController.hpp"
#include <luabind/luabind.hpp>
#include <luabind/iterator_policy.hpp>

namespace jar
{
    void InputDeviceManager::Luabind(lua_State* state)
    {
        luabind::module(state, "jar")
        [
            luabind::class_<InputDeviceManager>("InputDeviceManager")
                .def("GetAllControllers", &InputDeviceManager::GetAllControllers, luabind::return_stl_iterator) //Attention: cannot be nested! TODO: create lua table instead
        ];
    }
}
