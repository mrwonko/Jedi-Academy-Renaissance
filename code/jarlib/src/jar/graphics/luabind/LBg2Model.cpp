#include "jar/graphics/g2Model.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp>

namespace jar
{
namespace g2
{
void Model::BindToLua(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::namespace_("g2")
        [
            luabind::class_<Model>("Model")
                .def(luabind::constructor<>())
                .def("LoadFromFile", &LoadFromFile, luabind::pure_out_value(_3))
                .def("UploadToGPU", &UploadToGPU)
                .def("DeleteFromGPU", &DeleteFromGPU)
                .def("Render", &Render)
        ]
    ];
}

}
}
