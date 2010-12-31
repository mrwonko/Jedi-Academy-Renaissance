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

#include "jar/graphics/RenderWindow.hpp"
#include "jar/Input.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/input/Event.hpp"
#include "jar/input/EventManager.hpp"

namespace jar {

RenderWindow::RenderWindow(const unsigned int width, const unsigned int height, const std::string& title, const bool border, const bool fullscreen) :
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
