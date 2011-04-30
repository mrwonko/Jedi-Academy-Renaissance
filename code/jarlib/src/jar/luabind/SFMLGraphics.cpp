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
#include "jar/core/FileSystem.hpp"

#include "SFML/Graphics/Shape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Image.hpp"
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

const bool ImageLoadFromFile(sf::Image& image, const std::string& filename)
{
    std::string content;
    if(!fs::ReadFile(filename, content) &&
       !fs::ReadFile(filename + ".tga", content) &&
       !fs::ReadFile(filename + ".png", content) &&
       !fs::ReadFile(filename + ".jpg", content)
       ) return false;
    if(!image.LoadFromMemory(content.c_str(), content.length())) return false;
    return true;
}

/*
static std::string GetStringText(const sf::String& str)
{
    return str.GetText();
}
*/

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
            .def(luabind::constructor<sf::Uint8, sf::Uint8, sf::Uint8, sf::Uint8>())
            .def_readwrite("r", &sf::Color::r)
            .def_readwrite("g", &sf::Color::g)
            .def_readwrite("b", &sf::Color::b)
            .def_readwrite("a", &sf::Color::a),

        luabind::class_<sf::FloatRect>("FloatRect")
            .def(luabind::constructor<>())
            .def(luabind::constructor<float, float, float, float>())
            .def_readwrite("height", &sf::FloatRect::Height)
            .def_readwrite("width", &sf::FloatRect::Width)
            .def_readwrite("left", &sf::FloatRect::Left)
            .def_readwrite("top", &sf::FloatRect::Top)
            .def("Contains", (bool(sf::FloatRect::*)(float, float) const)&sf::FloatRect::Contains),
            //.def("Intersects", &sf::FloatRect::Intersects),

        luabind::class_<sf::IntRect>("IntRect")
            .def(luabind::constructor<>())
            .def(luabind::constructor<int, int, int, int>())
            .def_readwrite("height", &sf::IntRect::Height)
            .def_readwrite("width", &sf::IntRect::Width)
            .def_readwrite("left", &sf::IntRect::Left)
            .def_readwrite("top", &sf::IntRect::Top)
            .def("Contains", (bool(sf::IntRect::*)(int, int) const)&sf::IntRect::Contains),
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
            .def(luabind::constructor<const sf::View&>())
            .def(luabind::constructor<const sf::FloatRect&>())
            .def(luabind::constructor<const sf::Vector2f&, const sf::Vector2f&>())
            //overloaded function
            .def("SetCenter", (void(sf::View::*)(const sf::Vector2f&))&sf::View::SetCenter)
            .def("SetCenter", (void(sf::View::*)(float, float))&sf::View::SetCenter)
            .def("SetSize", (void(sf::View::*)(const sf::Vector2f&))&sf::View::SetSize)
            .def("SetSize", (void(sf::View::*)(float, float))&sf::View::SetSize)
            .def("Move", (void(sf::View::*)(const sf::Vector2f&))&sf::View::Move)
            .def("Move", (void(sf::View::*)(float, float))&sf::View::Move)
            .def("GetCenter", &sf::View::GetCenter)
            .def("GetSize", &sf::View::GetSize)
            .def("Zoom", &sf::View::Zoom),

        luabind::class_<sf::Drawable>("Drawable")
            .def("SetPosition", (void(sf::Drawable::*)(float, float))&sf::Drawable::SetPosition)
            .def("SetPosition", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::SetPosition)
            .def("SetX", &sf::Drawable::SetX)
            .def("SetY", &sf::Drawable::SetY)
            .def("SetScale", (void(sf::Drawable::*)(float, float))&sf::Drawable::SetScale)
            .def("SetScale", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::SetScale)
            .def("SetOrigin", (void(sf::Drawable::*)(float, float))&sf::Drawable::SetOrigin)
            .def("SetOrigin", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::SetOrigin)
            .def("SetScaleX", &sf::Drawable::SetScaleX)
            .def("SetScaleY", &sf::Drawable::SetScaleY)
            .def("SetRotation", &sf::Drawable::SetRotation)
            .def("SetColor", &sf::Drawable::SetColor)
            .def("SetBlendMode", &sf::Drawable::SetBlendMode)
            .def("GetPosition", &sf::Drawable::GetPosition)
            .def("GetScale", &sf::Drawable::GetScale)
            .def("GetOrigin", &sf::Drawable::GetOrigin)
            .def("GetRotation", &sf::Drawable::GetRotation)
            .def("GetColor", &sf::Drawable::GetColor)
            .def("GetBlendMode", &sf::Drawable::GetBlendMode)
            .def("Move", (void(sf::Drawable::*)(float, float))&sf::Drawable::Move)
            .def("Move", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::Move)
            .def("Scale", (void(sf::Drawable::*)(float, float))&sf::Drawable::Scale)
            .def("Scale", (void(sf::Drawable::*)(const sf::Vector2f&))&sf::Drawable::Scale)
            .def("Rotate", &sf::Drawable::Rotate)
            .def("TransformToLocal", &sf::Drawable::TransformToLocal)
            .def("TransformToGlobal", &sf::Drawable::TransformToGlobal),

        luabind::class_<sf::Shape, sf::Drawable>("Shape")
            .scope
            [
                luabind::def("Rectangle", (sf::Shape(*)(float, float, float, float, const sf::Color&, float, const sf::Color&)) &sf::Shape::Rectangle),
                luabind::def("Rectangle", (sf::Shape(*)(const sf::FloatRect&, const sf::Color&, float, const sf::Color&)) &sf::Shape::Rectangle),
                luabind::def("Line", (sf::Shape(*)(float, float, float, float, float, const sf::Color&, float, const sf::Color&)) &sf::Shape::Line),
                luabind::def("Line", (sf::Shape(*)(const sf::Vector2f&, const sf::Vector2f&, float, const sf::Color&, float, const sf::Color&)) &sf::Shape::Line),
                luabind::def("Circle", (sf::Shape(*)(float, float, float, const sf::Color&, float, const sf::Color&)) &sf::Shape::Circle),
                luabind::def("Circle", (sf::Shape(*)(const sf::Vector2f&,float, const sf::Color&, float, const sf::Color&)) &sf::Shape::Circle)
            ],

        luabind::class_<sf::Image>("Image")
            .def(luabind::constructor<>())
            .def("LoadFromFile", &ImageLoadFromFile)
            .def("SetSmooth", &sf::Image::SetSmooth),

        luabind::class_<sf::Sprite, sf::Drawable>("Sprite")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const sf::Image&>())
            .def(luabind::constructor<const sf::Image&, const sf::Vector2f&>())
            .def("SetImage", &sf::Sprite::SetImage)
            .def("GetImage", &sf::Sprite::GetImage)
            .def("FlipX", &sf::Sprite::FlipX)
            .def("FlipY", &sf::Sprite::FlipY)
            .def("SetSubRect", &sf::Sprite::SetSubRect)
            .def("GetSubRect", &sf::Sprite::GetSubRect)
            .def("Resize", (void(sf::Sprite::*)(const sf::Vector2f&))&sf::Sprite::Resize)
            .def("Resize", (void(sf::Sprite::*)(float, float))&sf::Sprite::Resize)
            .def("GetSize", &sf::Sprite::GetSize)

    ];

    luabind::globals(L)["jar"]["Color"]["Black"] = sf::Color::Black;
    luabind::globals(L)["jar"]["Color"]["Blue"] = sf::Color::Blue;
    luabind::globals(L)["jar"]["Color"]["Cyan"] = sf::Color::Cyan;
    luabind::globals(L)["jar"]["Color"]["Green"] = sf::Color::Green;
    luabind::globals(L)["jar"]["Color"]["Magenta"] = sf::Color::Magenta;
    luabind::globals(L)["jar"]["Color"]["Red"] = sf::Color::Red;
    luabind::globals(L)["jar"]["Color"]["White"] = sf::Color::White;
    luabind::globals(L)["jar"]["Color"]["Yellow"] = sf::Color::Yellow;
}

}
