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
