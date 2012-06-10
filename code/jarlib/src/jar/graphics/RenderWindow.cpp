#include "jar/graphics/RenderWindow.hpp"
#include "jar/Input.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/input/Event.hpp"
#include "jar/input/EventManager.hpp"
#include <SFML/Window/Event.hpp>
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"
#include <GL/glew.h>

namespace jar {

RenderWindow::RenderWindow(const unsigned int width, const unsigned int height, const std::string& title, const bool border, const bool fullscreen) :
    sf::RenderWindow(sf::VideoMode(width, height, 32), title, (fullscreen ? sf::Style::Fullscreen : (border ? sf::Style::Close : 0) ) )
{
    //ctor
    Input::GetSingleton().OnWindowCreated();

    // Initialize GLEW
    GLenum glewStatus = glewInit();
    if(glewStatus != GLEW_OK)
    {
        Logger::GetDefaultLogger().Error(std::string("Could not initialize GLEW: ") + reinterpret_cast<const char*>(glewGetErrorString(glewStatus)));
    }
}

RenderWindow::~RenderWindow()
{
    //dtor
}

void RenderWindow::Update(const TimeType deltaT)
{
    sf::Event e;
    while(sf::RenderWindow::pollEvent(e))
    {
        jar::Event sendMe;
        if(sendMe.FromSFML(e))
        {
            if(EventManager::HasSingleton())
            {
                EventManager::GetSingleton().ReceiveEvent(sendMe);
            }
        }
    }
}

} // namespace jar
