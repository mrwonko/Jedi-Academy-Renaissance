#include "jar/luabind/SFMLGraphics.hpp"
#include "jar/core/FileSystem.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/copy_policy.hpp>
#include <luabind/wrapper_base.hpp>

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
    if(!texture.loadFromMemory(content.c_str(), content.length())) return false;
    return true;
}

namespace
{
    void TransformableSetX(sf::Transformable& t, const float x)
    {
        t.setPosition(x, t.getPosition().y);
    }

    void TransformableSetY(sf::Transformable& t, const float y)
    {
        t.setPosition(t.getPosition().x, y);
    }

	struct sfDrawableHelper : sf::Drawable
	{
		sfDrawableHelper() {}

		// workaround for inability to get pointers to protected members
		virtual void draw2(sf::RenderTarget& target, const sf::RenderStates& states) const
		{
		}

	protected:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			draw2(target, states);
		}
	};

	struct sfDrawableWrapper : sfDrawableHelper, luabind::wrap_base
	{
		virtual void draw2(sf::RenderTarget& target, const sf::RenderStates& states) const
		{
			call<void>("Draw", boost::ref(target), states);
		}

		static void default_draw2(sfDrawableWrapper* ptr, sf::RenderTarget& target, const sf::RenderStates& states)
		{
			return ptr->draw2(target, states);
		}
	};
}

/*
static std::string GetStringText(const sf::String& str)
{
    return str.GetText();
}
*/

void BindSFMLGraphics(lua_State* L)
{
    typedef unsigned int tempUnsignedInt;
    luabind::module(L, "jar")
    [
        luabind::class_<sf::Vector2f>("Vector2f")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const sf::Vector2f&>())
            .def(luabind::constructor<float, float>())
            .def_readwrite("X", &sf::Vector2f::x)
            .def_readwrite("Y", &sf::Vector2f::y)
            .def("__unm", (sf::Vector2f (*)(const sf::Vector2f&))&sf::operator-)
            .def(luabind::self + luabind::other<sf::Vector2f>())
            .def(luabind::self - luabind::other<sf::Vector2f>())
            .def(luabind::self * float())
            .def(luabind::self / float())
            .def(luabind::self == luabind::other<sf::Vector2f>()),

        luabind::class_<sf::Vector2i>("Vector2i")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const sf::Vector2i&>())
            .def(luabind::constructor<int, int>())
            .def_readwrite("X", &sf::Vector2i::x)
            .def_readwrite("Y", &sf::Vector2i::y)
            .def("__unm", (sf::Vector2i (*)(const sf::Vector2i&))&sf::operator-)
            .def(luabind::self + luabind::other<sf::Vector2i>())
            .def(luabind::self - luabind::other<sf::Vector2i>())
            .def(luabind::self * int())
            .def(luabind::self / int())
            .def(luabind::self == luabind::other<sf::Vector2i>()),

        luabind::class_<sf::Vector2u>("Vector2u")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const sf::Vector2u&>())
            .def(luabind::constructor<unsigned int, unsigned int>())
            .def_readwrite("X", &sf::Vector2u::x)
            .def_readwrite("Y", &sf::Vector2u::y)
            //unary minus makes no sense for vector2<unsigned int>
            .def(luabind::self + luabind::other<sf::Vector2u>())
            .def(luabind::self - luabind::other<sf::Vector2u>())
            .def(luabind::self * tempUnsignedInt())
            .def(luabind::self / tempUnsignedInt())
            .def(luabind::self == luabind::other<sf::Vector2u>()),

        luabind::class_<sf::Color>("Color")
            .def(luabind::constructor<>())
            .def(luabind::constructor<sf::Uint8, sf::Uint8, sf::Uint8>())
            .def(luabind::constructor<sf::Uint8, sf::Uint8, sf::Uint8, sf::Uint8>())
            .def_readwrite("R", &sf::Color::r)
            .def_readwrite("G", &sf::Color::g)
            .def_readwrite("B", &sf::Color::b)
            .def_readwrite("A", &sf::Color::a),

        luabind::class_<sf::FloatRect>("FloatRect")
            .def(luabind::constructor<>())
            .def(luabind::constructor<float, float, float, float>())
            .def_readwrite("height", &sf::FloatRect::height)
            .def_readwrite("width", &sf::FloatRect::width)
            .def_readwrite("left", &sf::FloatRect::left)
            .def_readwrite("top", &sf::FloatRect::top)
            .def("Contains", (bool(sf::FloatRect::*)(float, float) const)&sf::FloatRect::contains)
            //todo: document!
            .def("Intersects", (bool(sf::FloatRect::*)(const sf::FloatRect&, sf::FloatRect&) const)&sf::FloatRect::intersects, luabind::pure_out_value(_3)),

        luabind::class_<sf::IntRect>("IntRect")
            .def(luabind::constructor<>())
            .def(luabind::constructor<int, int, int, int>())
            .def_readwrite("height", &sf::IntRect::height)
            .def_readwrite("width", &sf::IntRect::width)
            .def_readwrite("left", &sf::IntRect::left)
            .def_readwrite("top", &sf::IntRect::top)
            .def("Contains", (bool(sf::IntRect::*)(int, int) const)&sf::IntRect::contains)
            //todo: document!
            .def("Intersects", (bool(sf::IntRect::*)(const sf::IntRect&, sf::IntRect&) const)&sf::IntRect::intersects, luabind::pure_out_value(_3)), //_1 = this or sth

        luabind::class_<luabind_dummy::Blend>("Blend")
            .enum_("Mode")
            [
                luabind::value("Alpha", (unsigned long)sf::BlendAlpha),
                luabind::value("Add", (unsigned long)sf::BlendAdd),
                luabind::value("Multiply", (unsigned long)sf::BlendMultiply),
                luabind::value("None", (unsigned long)sf::BlendNone)
            ],

        luabind::class_<sf::View>("View")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const sf::View&>())
            .def(luabind::constructor<const sf::FloatRect&>())
            .def(luabind::constructor<const sf::Vector2f&, const sf::Vector2f&>())
            //overloaded function
            .def("SetCenter", (void(sf::View::*)(const sf::Vector2f&))&sf::View::setCenter)
            .def("SetCenter", (void(sf::View::*)(float, float))&sf::View::setCenter)
            .def("SetSize", (void(sf::View::*)(const sf::Vector2f&))&sf::View::setSize)
            .def("SetSize", (void(sf::View::*)(float, float))&sf::View::setSize)
            .def("Move", (void(sf::View::*)(const sf::Vector2f&))&sf::View::move)
            .def("Move", (void(sf::View::*)(float, float))&sf::View::move)
            .def("GetCenter", &sf::View::getCenter, luabind::copy(luabind::result))
            .def("GetSize", &sf::View::getSize, luabind::copy(luabind::result))
            .def("Zoom", &sf::View::zoom)
            .def("SetViewport", &sf::View::setViewport)
			.def("GetViewport", &sf::View::getViewport, luabind::copy(luabind::result)),

        luabind::class_<sf::Transformable>("Transformable2D")
            .def("SetPosition", (void(sf::Transformable::*)(float, float))&sf::Transformable::setPosition)
            .def("SetPosition", (void(sf::Transformable::*)(const sf::Vector2f&))&sf::Transformable::setPosition)
            .def("SetX", &TransformableSetX)
            .def("SetY", &TransformableSetY)
            .def("SetScale", (void(sf::Transformable::*)(float, float))&sf::Transformable::setScale)
            .def("SetScale", (void(sf::Transformable::*)(const sf::Vector2f&))&sf::Transformable::setScale)
            .def("SetOrigin", (void(sf::Transformable::*)(float, float))&sf::Transformable::setOrigin)
            .def("SetOrigin", (void(sf::Transformable::*)(const sf::Vector2f&))&sf::Transformable::setOrigin)
            .def("SetRotation", &sf::Transformable::setRotation)
            .def("GetPosition", &sf::Transformable::getPosition, luabind::copy(luabind::result))
            .def("GetScale", &sf::Transformable::getScale, luabind::copy(luabind::result))
            .def("GetOrigin", &sf::Transformable::getOrigin, luabind::copy(luabind::result))
            .def("GetRotation", &sf::Transformable::getRotation, luabind::copy(luabind::result))
            .def("Move", (void(sf::Transformable::*)(float, float))&sf::Transformable::move)
            .def("Move", (void(sf::Transformable::*)(const sf::Vector2f&))&sf::Transformable::move)
            .def("Scale", (void(sf::Transformable::*)(float, float))&sf::Transformable::scale)
            .def("Scale", (void(sf::Transformable::*)(const sf::Vector2f&))&sf::Transformable::scale)
            .def("Rotate", &sf::Transformable::rotate),

		luabind::class_<sf::Transform>("Transform2D")
			.def(luabind::constructor<>())
			.def("Translate", (sf::Transform& (sf::Transform::*) (float, float)) &sf::Transform::translate, luabind::return_reference_to(_1))
			.def("Translate", (sf::Transform& (sf::Transform::*) (const sf::Vector2f&)) &sf::Transform::translate, luabind::return_reference_to(_1))
			.def("Rotate", (sf::Transform& (sf::Transform::*) (float)) &sf::Transform::rotate, luabind::return_reference_to(_1)),

		// only used as parameter in drawable, opaque
        luabind::class_<sf::RenderStates>("RenderStates")
			.def_readwrite("Transform", &sf::RenderStates::transform),

        luabind::class_<sf::Drawable>("_Drawable2D"),

		luabind::class_<sfDrawableHelper, luabind::bases<sf::Drawable>, sfDrawableWrapper>("Drawable2D")
			.def(luabind::constructor<>())
			.def("Draw", &sfDrawableHelper::draw2, &sfDrawableWrapper::default_draw2),

        luabind::class_<sf::Shape, luabind::bases<sf::Drawable, sf::Transformable> >("Shape")
            .def("SetFillColor", &sf::Shape::setFillColor)
            .def("SetOutlineColor", &sf::Shape::setOutlineColor)
            .def("SetOutlineThickness", &sf::Shape::setOutlineThickness)
            .def("GetFillColor", &sf::Shape::getFillColor, luabind::copy(luabind::result))
            .def("GetOutlineColor", &sf::Shape::getOutlineColor, luabind::copy(luabind::result))
            .def("GetOutlineThickness", &sf::Shape::getOutlineThickness, luabind::copy(luabind::result)),

        luabind::class_<sf::CircleShape, sf::Shape>("CircleShape")
            .def(luabind::constructor<float>())
            .def(luabind::constructor<float, unsigned int>())
            .def("SetRadius", &sf::CircleShape::setRadius)
            .def("GetRadius", &sf::CircleShape::getRadius),

        luabind::class_<sf::RectangleShape, sf::Shape>("RectangleShape")
            .def(luabind::constructor<sf::Vector2f>())
            .def("SetSize", &sf::RectangleShape::setSize)
            .def("GetSize", &sf::RectangleShape::getSize, luabind::copy(luabind::result)),

        luabind::class_<sf::Texture>("Texture")
            .def(luabind::constructor<>())
            .def("LoadFromFile", &TextureLoadFromFile)
            .def("GetSize", &sf::Texture::getSize)
            .def("SetSmooth", &sf::Texture::setSmooth),

        luabind::class_<sf::Sprite, luabind::bases<sf::Drawable, sf::Transformable> >("Sprite")
            .def(luabind::constructor<>())
            .def(luabind::constructor<const sf::Texture&>())
            .def("SetTexture", &sf::Sprite::setTexture)
            .def("GetTexture", &sf::Sprite::getTexture)
            .def("SetTextureRect", &sf::Sprite::setTextureRect)
            .def("GetTextureRect", &sf::Sprite::getTextureRect, luabind::copy(luabind::result))
            .def("GetGlobalBounds", &sf::Sprite::getGlobalBounds, luabind::copy(luabind::result))
            .def("SetColor", &sf::Sprite::setColor)
            .def("GetColor", &sf::Sprite::getColor)

    ];

    luabind::globals(L)["jar"]["Color"]["Black"] = sf::Color::Black;
    luabind::globals(L)["jar"]["Color"]["Blue"] = sf::Color::Blue;
    luabind::globals(L)["jar"]["Color"]["Cyan"] = sf::Color::Cyan;
    luabind::globals(L)["jar"]["Color"]["Green"] = sf::Color::Green;
    luabind::globals(L)["jar"]["Color"]["Magenta"] = sf::Color::Magenta;
    luabind::globals(L)["jar"]["Color"]["Red"] = sf::Color::Red;
    luabind::globals(L)["jar"]["Color"]["White"] = sf::Color::White;
    luabind::globals(L)["jar"]["Color"]["Yellow"] = sf::Color::Yellow;
    luabind::globals(L)["jar"]["Color"]["Transparent"] = sf::Color::Transparent;
}

}
