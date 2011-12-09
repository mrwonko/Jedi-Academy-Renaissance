#include "jar/core/Time.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/graphics/RenderWindow.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Image.hpp>

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

void SetCursorPosition(const RenderWindow& window, const sf::Vector2i& position)
{
    sf::Mouse::SetPosition(position, window);
}

void RenderWindow::Luabind(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<RenderWindow>("RenderWindow")
            .def(luabind::constructor<const unsigned int, const unsigned int, const std::string&, const bool, const bool>())
            .def("Clear", &jar::RenderWindow::Clear)
            .def("Draw", (void(jar::RenderWindow::*)(const sf::Drawable&))&jar::RenderWindow::Draw)
            .def("SetView", &jar::RenderWindow::SetView)
            .def("GetView", &jar::RenderWindow::GetView)
            .def("GetDefaultView", &jar::RenderWindow::GetDefaultView)
            //NOTE: no longer exists in SFML 2.0 - now there are SaveGLStates() and RestoreGLStates() - need to know what they do first.
            //.def("PreserveOpenGLStates", &jar::RenderWindow::PreserveOpenGLStates)


            //NOTE: this either does not yet exist in 2.0 or I just haven't found it - ok found it, it's Image::CopyFromScreen() or sth
            //.def("Capture", &RenderWindow::Capture)

            .def("Close", &jar::RenderWindow::Close)
            .def("Display", &jar::RenderWindow::Display)
            .def("EnableKeyRepeat", &jar::RenderWindow::EnableKeyRepeat)
            .def("GetFrameTime", &jar::GetFrameTime)
            .def("GetHeight", &jar::RenderWindow::GetHeight)
            //maybe later...?
            //.def("GetSettings", &jar::RenderWindow::GetSettings)
            .def("GetWidth", &jar::RenderWindow::GetWidth)
            .def("SetActive", &jar::RenderWindow::SetActive)
//            .def("SetCursorPosition", &jar::RenderWindow::SetCursorPosition)
            .def("SetCursorPosition", &SetCursorPosition)
            .def("SetFramerateLimit", &jar::RenderWindow::SetFramerateLimit)
            .def("SetIcon", &RenderWindowSetIcon)
            .def("SetPosition", &jar::RenderWindow::SetPosition)
            .def("SetSize", &jar::RenderWindow::SetSize)
            .def("Show", &jar::RenderWindow::Show)
            .def("ShowMouseCursor", &jar::RenderWindow::ShowMouseCursor)
            //NOTE: this either does not yet exist in 2.0 or I just haven't found it
            //.def("UseVerticalSync", &jar::RenderWindow::UseVerticalSync)
    ];
}

}
