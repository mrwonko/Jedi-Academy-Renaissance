/*
===========================================================================
Copyright (C) 2010 Willi Schinmeyer

This file is part of the Jedi Academy: Renaissance source code.

Jedi Academy: Renaissance source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Jedi Academy: Renaissance source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jedi Academy: Renaissance source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

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
        luabind::class_<Text, sf::Drawable>("Text")
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
    ];
}
}
