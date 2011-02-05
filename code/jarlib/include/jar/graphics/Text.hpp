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

#ifndef JAR_GRAPHICS_TEXT_HPP
#define JAR_GRAPHICS_TEXT_HPP

#include "jar/graphics/API.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <string>

struct lua_State;

namespace jar {

class Font;

class JARGRAPHICSAPI Text : public sf::Drawable
{
    public:
        /** \note don't use, just here so autopointers work **/
        Text();
        Text(const Font& font);
        Text(const Font& font, const unsigned int fontSize);
        virtual ~Text();

        /** \brief Sets the font, does not change the size **/
        void SetFont(const Font& font);

        /** \brief Gets the font **/
        const Font& GetFont() const;

        /** \brief Sets the text **/
        void SetText(const std::string& text);

        /** \brief Gets the text **/
        const std::string& GetText() const;

        /** \brief Sets the font size, in points **/
        void SetFontSize(const float size);

        /** \brief Gets the font size, in points (default: font's size) **/
        const float GetFontSize() const;

        /** \brief Gets the width of this text, in pixels - depends on font and fontsize **/
        const float GetWidth() const;

        virtual void Render(sf::RenderTarget& target) const;

        static void BindToLua(lua_State* L);
    protected:
    private:
        std::string mText;
        const Font* mFont;
        float mFontSize;
};

} // namespace jar

#endif // JAR_GRAPHICS_TEXT_HPP
