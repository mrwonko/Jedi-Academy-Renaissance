#include "jar/graphics/RenderWindow.hpp"
#include "jar/Input.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/input/Event.hpp"
#include "jar/input/EventManager.hpp"

namespace jar {

RenderWindow::RenderWindow(unsigned int width, unsigned int height, const std::string& title, const bool border, const bool fullscreen) :
    sf::RenderWindow(sf::VideoMode(width, height, 32), title, (fullscreen ? sf::Style::Fullscreen : (border ? sf::Style::Close : 0) ) )
{
    //ctor
}

RenderWindow::~RenderWindow()
{
    //dtor
}

void RenderWindow::Update(const TimeType deltaT)
{
    sf::Event e;
    while(sf::RenderWindow::GetEvent(e))
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
