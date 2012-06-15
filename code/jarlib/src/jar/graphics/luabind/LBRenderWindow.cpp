#include "jar/core/Time.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/graphics/RenderWindow.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <luabind/copy_policy.hpp>

namespace jar
{

namespace
{
    void RenderWindowSetIcon(jar::RenderWindow& window, const sf::Texture& texture)
    {
        sf::Image img(texture.copyToImage());
        window.setIcon(img.getSize().x, img.getSize().y, img.getPixelsPtr());
    }

    void SetCursorPosition(const RenderWindow& window, const sf::Vector2i& position)
    {
        //TODO/FIXME: what coordinate system is used here?
        sf::Mouse::setPosition(position, window);
    }

    void RenderTargetDraw(sf::RenderTarget& target, const sf::Drawable& drawable)
    {
        target.draw(drawable);
    }
}

void RenderWindow::Luabind(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<sf::RenderTarget>("RenderTarget")
            .def("SetView", &sf::RenderTarget::setView)
            .def("GetView", &sf::RenderTarget::getView, luabind::copy(luabind::result))
            .def("GetDefaultView", &sf::RenderTarget::getDefaultView, luabind::copy(luabind::result))
            .def("GetSize", &sf::RenderTarget::getSize, luabind::copy(luabind::result))
			.def("Draw", &RenderTargetDraw)
			.def("Draw", (void(sf::RenderTarget::*)(const sf::Drawable&, const sf::RenderStates&))&sf::RenderTarget::draw),

        luabind::class_<RenderWindow, sf::RenderTarget>("RenderWindow")
            .def(luabind::constructor<const unsigned int, const unsigned int, const std::string&, const bool, const bool>())
            .def("Clear", &jar::RenderWindow::clear)
            //NOTE: no longer exists in SFML 2.0 - now there are SaveGLStates() and RestoreGLStates() - need to know what they do first.
            //.def("PreserveOpenGLStates", &jar::RenderWindow::PreserveOpenGLStates)


            //NOTE: this either does not yet exist in 2.0 or I just haven't found it - ok found it, it's Image::CopyFromScreen() or sth
            //.def("Capture", &RenderWindow::Capture)
			
            .def("SetSize", &sf::RenderWindow::setSize)
            .def("Close", &jar::RenderWindow::close)
            .def("Display", &jar::RenderWindow::display)
            .def("SetKeyRepeatEnabled", &jar::RenderWindow::setKeyRepeatEnabled) //todo: create jar.Keyboard for this?
            //maybe later...?
            //.def("GetSettings", &jar::RenderWindow::GetSettings)
            .def("SetActive", &jar::RenderWindow::setActive)
//            .def("SetCursorPosition", &jar::RenderWindow::SetCursorPosition)
            .def("SetCursorPosition", &SetCursorPosition)
            .def("SetFramerateLimit", &jar::RenderWindow::setFramerateLimit)
            .def("SetIcon", &RenderWindowSetIcon)
            .def("SetPosition", &jar::RenderWindow::setPosition)
            .def("SetMouseCursorVisible", &jar::RenderWindow::setMouseCursorVisible)
            //NOTE: this either does not yet exist in 2.0 or I just haven't found it
            //.def("UseVerticalSync", &jar::RenderWindow::UseVerticalSync)
    ];
}

}
