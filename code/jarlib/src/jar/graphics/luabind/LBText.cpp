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
		// TODO: Change this to store a luabind::object to keep the Font alive, so font changes don't leak memory
        luabind::class_<Text, luabind::bases<sf::Drawable, sf::Transformable> >("Text")
            //since fonts are stored as a pointer, they better not be deleted while texts still use them.
            //first parameter: who keeps alive? second parameter: who is kept alive? (_1 is "this")
            .def(luabind::constructor<const luabind::object&>())
            .def(luabind::constructor<const luabind::object&, const unsigned int>())
            .def("SetText", &Text::SetText)
            .def("GetText", &Text::GetText)
            .def("SetFont", &Text::SetFont)
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
