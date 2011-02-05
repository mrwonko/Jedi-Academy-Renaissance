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

#include "jar/core/Time.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/graphics/RenderWindow.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>

#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/Graphics/Image.hpp"

namespace jar
{

static TimeType GetFrameTime(const jar::RenderWindow& window)
{
    //on high fps rounding may always result in 0, so I need to keep track of the time lost through rounding.
    static float dif = 0;

    float time = window.GetFrameTime() * TimeTypesPerSecond;

    time += dif;

    TimeType roundedTime = jar::Helpers::Round(time);

    dif = time - roundedTime;

    return roundedTime;
}

static void RenderWindowSetIcon(jar::RenderWindow& window, const sf::Image& image)
{
    window.SetIcon(image.GetWidth(), image.GetHeight(), image.GetPixelsPtr());
}

void RenderWindow::Luabind(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<RenderWindow>("RenderWindow")
            .def(luabind::constructor<const unsigned int, const unsigned int, const std::string&, const bool, const bool>())
            .def("Clear", &jar::RenderWindow::Clear)
            .def("Draw", &jar::RenderWindow::Draw)
            .def("SetView", &jar::RenderWindow::SetView)
            .def("GetView", &jar::RenderWindow::GetView)
            .def("GetDefaultView", &jar::RenderWindow::GetDefaultView)
            .def("PreserveOpenGLStates", &jar::RenderWindow::PreserveOpenGLStates)


            .def("Capture", &RenderWindow::Capture)

            .def("Close", &jar::RenderWindow::Close)
            .def("Display", &jar::RenderWindow::Display)
            .def("EnableKeyRepeat", &jar::RenderWindow::EnableKeyRepeat)
            .def("GetFrameTime", &jar::GetFrameTime)
            .def("GetHeight", &jar::RenderWindow::GetHeight)
            //maybe later...?
            //.def("GetSettings", &jar::RenderWindow::GetSettings)
            .def("GetWidth", &jar::RenderWindow::GetWidth)
            .def("SetActive", &jar::RenderWindow::SetActive)
            .def("SetCursorPosition", &jar::RenderWindow::SetCursorPosition)
            .def("SetFramerateLimit", &jar::RenderWindow::SetFramerateLimit)
            .def("SetIcon", &RenderWindowSetIcon)
            .def("SetPosition", &jar::RenderWindow::SetPosition)
            .def("SetSize", &jar::RenderWindow::SetSize)
            .def("Show", &jar::RenderWindow::Show)
            .def("ShowMouseCursor", &jar::RenderWindow::ShowMouseCursor)
            .def("UseVerticalSync", &jar::RenderWindow::UseVerticalSync)
    ];
}

}
