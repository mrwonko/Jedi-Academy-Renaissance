#include "jar/graphics/Text.hpp"
#include "jar/graphics/Font.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/dependency_policy.hpp>


namespace jar
{
void Text::BindToLua(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<Text, luabind::bases<sf::Drawable, sf::Transformable> >("Text")
            //since fonts are stored as a pointer, they better not be deleted while texts still use them.
            //first parameter: who keeps alive? second parameter: who is kept alive? (_1 is "this")
            .def(luabind::constructor<const Font&>(), luabind::dependency(_1, _2))
            .def(luabind::constructor<const Font&, const unsigned int>(), luabind::dependency(_1, _2))
            .def("SetText", &Text::SetText)
            .def("GetText", &Text::GetText)
            .def("SetFont", &Text::SetFont, luabind::dependency(_1, _2))
            .def("GetFont", &Text::GetFont)
            .def("SetFontSize", &Text::SetFontSize)
            .def("GetFontSize", &Text::GetFontSize)
            .def("GetWidth", &Text::GetWidth)
            .def("GetLineHeight", &Text::GetLineHeight)
            .def("GetHeight", &Text::GetHeight)
            .def("GetSize", &Text::GetSize)
    ];
}
}
