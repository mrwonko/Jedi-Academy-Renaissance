#include "jar/graphics/Font.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace jar
{
void Font::BindToLua(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<Font>("Font")
            .def(luabind::constructor<>())
            .def("LoadFromFile", &Font::LoadFromFile)
    ];
}
}
