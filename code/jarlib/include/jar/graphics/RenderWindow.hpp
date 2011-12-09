#ifndef JAR_GRAPHICS_RENDERWINDOW_HPP
#define JAR_GRAPHICS_RENDERWINDOW_HPP

#include <SFML/Graphics/RenderWindow.hpp>

#include <jar/graphics/API.hpp>
#include <jar/input/EventQueue.hpp>
#include <jar/core/Updatable.hpp>
#include <string>

class lua_State;

namespace jar {

class JARGRAPHICSAPI RenderWindow : public sf::RenderWindow, public Updatable
{
    public:
        RenderWindow(const unsigned int width, const unsigned int height, const std::string& title, const bool border, const bool fullscreen = false);
        virtual ~RenderWindow();

        virtual void Update(const TimeType deltaT);

        static void Luabind(lua_State* L);
    protected:
    private:
        //don't call this, use the EventManager! (No, don't call RenderWindow::GetEvent() either, you'll only break things. Probably.)
        bool GetEvent(sf::Event&);
};

} // namespace jar

#endif // JAR_GRAPHICS_RENDERWINDOW_HPP
