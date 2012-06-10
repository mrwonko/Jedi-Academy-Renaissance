#include "jar/graphics/g2Model.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>


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
                .def("LoadFromFile", &LoadFromFile)
                .def("UploadToGPU", &UploadToGPU)
                .def("DeleteFromGPU", &DeleteFromGPU)
                .def("Render", &Render)
        ]
    ];
}

}
}
