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

#include "jar/luabind/SFMLGraphics.hpp"

#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/View.hpp"

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

namespace jar
{

//enums can only be added to classes (I think?), thus I need some dummy classes
namespace luabind_dummy
{
class Blend {};
}

void BindSFMLGraphics(lua_State* L)
{
    typedef sf::Vector2f (*funcptr1)(const sf::Vector2f&);
    luabind::module(L, "jar")
    [
        luabind::class_<sf::Vector2f>("Vector2f")
            .def(luabind::constructor<>())
            .def(luabind::constructor<float, float>())
            .def_readwrite("x", &sf::Vector2f::x)
            .def_readwrite("y", &sf::Vector2f::y)
            .def("Invert", (funcptr1)&sf::operator-)
            .def(luabind::self + luabind::other<sf::Vector2f>())
            .def(luabind::self - luabind::other<sf::Vector2f>())
            .def(luabind::self * float())
            .def(luabind::self / float())
            .def(luabind::self == luabind::other<sf::Vector2f>()),

        luabind::class_<sf::Color>("Color")
            .def(luabind::constructor<>())
            .def(luabind::constructor<sf::Uint8, sf::Uint8, sf::Uint8>())
            .def_readwrite("r", &sf::Color::r)
            .def_readwrite("g", &sf::Color::g)
            .def_readwrite("b", &sf::Color::b)
            .def_readwrite("a", &sf::Color::a),

        luabind::class_<sf::FloatRect>("FloatRect")
            .def(luabind::constructor<>())
            .def(luabind::constructor<float, float, float, float>())
            .def_readwrite("bottom", &sf::FloatRect::Bottom)
            .def_readwrite("left", &sf::FloatRect::Left)
            .def_readwrite("right", &sf::FloatRect::Right)
            .def_readwrite("top", &sf::FloatRect::Top)
            .def("GetWidth", &sf::FloatRect::GetWidth)
            .def("GetHeight", &sf::FloatRect::GetHeight)
            .def("Offset", &sf::FloatRect::Offset)
            .def("Contains", &sf::FloatRect::Contains),
            //.def("Intersects", &sf::FloatRect::Intersects),

        luabind::class_<sf::IntRect>("IntRect")
            .def(luabind::constructor<>())
            .def(luabind::constructor<int, int, int, int>())
            .def_readwrite("bottom", &sf::IntRect::Bottom)
            .def_readwrite("left", &sf::IntRect::Left)
            .def_readwrite("right", &sf::IntRect::Right)
            .def_readwrite("top", &sf::IntRect::Top)
            .def("GetWidth", &sf::IntRect::GetWidth)
            .def("GetHeight", &sf::IntRect::GetHeight)
            .def("Offset", &sf::IntRect::Offset)
            .def("Contains", &sf::IntRect::Contains),
            //.def("Intersects", &sf::IntRect::Intersects),

        luabind::class_<luabind_dummy::Blend>("Blend")
            .enum_("Mode")
            [
                luabind::value("Alpha", (unsigned long)sf::Blend::Alpha),
                luabind::value("Add", (unsigned long)sf::Blend::Add),
                luabind::value("Multiply", (unsigned long)sf::Blend::Multiply),
                luabind::value("None", (unsigned long)sf::Blend::None)
            ],

        luabind::class_<sf::View>("View")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const sf::FloatRect&>())
            .def(luabind::constructor<const sf::Vector2f&, const sf::Vector2f&>())
            //overloaded function
            .def("SetCenter", (void(sf::View::*)(const sf::Vector2f&))&sf::View::SetCenter)
            .def("SetCenter", (void(sf::View::*)(float, float))&sf::View::SetCenter)
            .def("SetHalfSize", (void(sf::View::*)(const sf::Vector2f&))&sf::View::SetHalfSize)
            .def("SetHalfSize", (void(sf::View::*)(float, float))&sf::View::SetHalfSize)
            .def("Move", (void(sf::View::*)(const sf::Vector2f&))&sf::View::Move)
            .def("Move", (void(sf::View::*)(float, float))&sf::View::Move)
            .def("SetFromRect", &sf::View::SetFromRect)
            .def("GetCenter", &sf::View::GetCenter)
            .def("GetHalfSize", &sf::View::GetHalfSize)
            .def("GetRect", &sf::View::GetRect)
            .def("Zoom", &sf::View::Zoom),

        luabind::class_<sf::Drawable>("Drawable")
            .def("SetPosition", (void(sf::Drawable::*)(float, float))&sf::Drawable::SetPosition)
            .def("SetPosition", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::SetPosition)
            .def("SetX", &sf::Drawable::SetX)
            .def("SetY", &sf::Drawable::SetY)
            .def("SetScale", (void(sf::Drawable::*)(float, float))&sf::Drawable::SetScale)
            .def("SetScale", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::SetScale)
            .def("SetCenter", (void(sf::Drawable::*)(float, float))&sf::Drawable::SetCenter)
            .def("SetCenter", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::SetCenter)
            .def("SetScaleX", &sf::Drawable::SetScaleX)
            .def("SetScaleY", &sf::Drawable::SetScaleY)
            .def("SetRotation", &sf::Drawable::SetRotation)
            .def("SetColor", &sf::Drawable::SetColor)
            .def("SetBlendMode", &sf::Drawable::SetBlendMode)
            .def("GetPosition", &sf::Drawable::GetPosition)
            .def("GetScale", &sf::Drawable::GetScale)
            .def("GetCenter", &sf::Drawable::GetCenter)
            .def("GetRotation", &sf::Drawable::GetRotation)
            .def("GetColor", &sf::Drawable::GetColor)
            .def("GetBlendMode", &sf::Drawable::GetBlendMode)
            .def("Move", (void(sf::Drawable::*)(float, float))&sf::Drawable::Move)
            .def("Move", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::Move)
            .def("Scale", (void(sf::Drawable::*)(float, float))&sf::Drawable::Scale)
            .def("Scale", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::Scale)
            .def("Rotate", &sf::Drawable::Rotate)
            .def("TransformToLocal", &sf::Drawable::TransformToLocal)
            .def("TransformToGlobal", &sf::Drawable::TransformToGlobal)
    ];
}

}
