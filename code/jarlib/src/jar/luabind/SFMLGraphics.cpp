#include "jar/luabind/SFMLGraphics.hpp"
#include "jar/core/FileSystem.hpp"

#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/View.hpp>

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/out_value_policy.hpp>

namespace jar
{

//enums can only be added to classes (I think?), thus I need some dummy classes
namespace luabind_dummy
{
class Blend {};
}

const bool TextureLoadFromFile(sf::Texture& texture, const std::string& filename)
{
    std::string content;
    if(!fs::ReadFile(filename, content) &&
       !fs::ReadFile(filename + ".tga", content) &&
       !fs::ReadFile(filename + ".png", content) &&
       !fs::ReadFile(filename + ".jpg", content)
       ) return false;
    if(!texture.LoadFromMemory(content.c_str(), content.length())) return false;
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
            .def(luabind::constructor<const sf::Vector2f&>())
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
            .def("Contains", (bool(sf::FloatRect::*)(float, float) const)&sf::FloatRect::Contains)
            //todo: document!
            .def("Intersects", (bool(sf::FloatRect::*)(const sf::FloatRect&, sf::FloatRect&) const)&sf::FloatRect::Intersects, luabind::pure_out_value(_3)),

        luabind::class_<sf::IntRect>("IntRect")
            .def(luabind::constructor<>())
            .def(luabind::constructor<int, int, int, int>())
            .def_readwrite("height", &sf::IntRect::Height)
            .def_readwrite("width", &sf::IntRect::Width)
            .def_readwrite("left", &sf::IntRect::Left)
            .def_readwrite("top", &sf::IntRect::Top)
            .def("Contains", (bool(sf::IntRect::*)(int, int) const)&sf::IntRect::Contains)
            //todo: document!
            .def("Intersects", (bool(sf::IntRect::*)(const sf::IntRect&, sf::IntRect&) const)&sf::IntRect::Intersects, luabind::pure_out_value(_3)), //_1 = this or sth

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
            .def("Zoom", &sf::View::Zoom)
            .def("SetViewport", &sf::View::SetViewport),

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

        luabind::class_<sf::Texture>("Texture")
            .def(luabind::constructor<>())
            .def("LoadFromFile", &TextureLoadFromFile)
            //TODO: document the following
            .def("GetHeight", &sf::Texture::GetHeight)
            .def("GetWidth", &sf::Texture::GetWidth)
            .def("SetSmooth", &sf::Texture::SetSmooth),

        luabind::class_<sf::Sprite, sf::Drawable>("Sprite")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const sf::Texture&>())
            .def("SetTexture", &sf::Sprite::SetTexture)
            .def("GetTexture", &sf::Sprite::GetTexture)
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
