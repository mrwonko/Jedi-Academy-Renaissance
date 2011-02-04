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

#include "jar/graphics/Font.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace jar
{
static short GetFontHeight(const Font& f)
{
    return f.GetFontData().mHeight;// + f.GetFontData().mDescender+1;
}

static short GetFontDefaultSize(const Font& f)
{
    return f.GetFontData().mPointSize;
}

void Font::BindToLua(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<Font>("Font")
            .def(luabind::constructor<>())
            .def("LoadFromFile", &Font::LoadFromFile)
            .def("GetWidth", &Font::GetWidth)
            .def("GetTabWidth", &Font::GetTabWidth)
            .def("SetTabWidth", &Font::SetTabWidth)
            .def("GetHeight", &GetFontHeight)
            .def("GetDefaultSize", &GetFontDefaultSize)
    ];
}
}
