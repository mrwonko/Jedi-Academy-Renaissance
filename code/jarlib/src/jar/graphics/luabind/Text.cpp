#include "jar/graphics/Text.hpp"
#include "jar/graphics/Font.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace jar
{
void Text::BindToLua(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<Text, sf::Drawable>("Text")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const Font&>())
            .def("SetText", &Text::SetText)
            .def("GetText", &Text::GetText)
            .def("SetFont", &Text::SetFont)
            .def("GetFont", &Text::GetFont)
            .def("SetFontSize", &Text::SetFontSize)
            .def("GetFontSize", &Text::GetFontSize)
    ];
}
}
