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

#ifndef JAR_GRAPHICS_FONT_HPP
#define JAR_GRAPHICS_FONT_HPP

#include "jar/graphics/API.hpp"
#include <string>

struct lua_State;

namespace sf
{
    class Image;
}

namespace jar {

class Text;

class JARGRAPHICSAPI Font
{
    public:
        Font();
        virtual ~Font();

        /** \param filename without extension - a *filename*.png/.tga/.jpg and a *filename*.fontdat should exist.
        **/
        const bool LoadFromFile(const std::string& filename);

        /** \brief Returns the width of a given string, in pixels, if it was written using this font.
        **/

        const unsigned int GetWidth(const std::string& str) const;

        static void BindToLua(lua_State* L);

        // from the JKA SDK
        // these structs corresponds 1:1 with the "*.fontdat" files, so don't change them
        struct GlyphInfo
        {
            short		mWidth;					// number of pixels wide
            short		mHeight;				// number of scan lines
            short		mHorizAdvance;			// number of pixels to advance to the next char
            short		mHorizOffset;			// x offset into space to render glyph
            int			mBaseline;				// y offset
            float		mTexCoordX1;			// x start tex coord
            float		mTexCoordY1;			// y start tex coord
            float		mTexCoordX2;			// x end tex coord
            float		mTexCoordY2;			// y end tex coord
        };

        struct FontData
        {
            static const unsigned int GLYPH_COUNT = 256;
            GlyphInfo	mGlyphs[GLYPH_COUNT];

            short		mPointSize;             //size of font in points
            short		mHeight;				// max height of font
            short		mAscender;
            short		mDescender;

            short		mKoreanHack;
        };

        const FontData& GetFontData() const { return mFontData; }

        const sf::Image& GetImage() const { return *mImage; }

        /** \brief Sets the width of tabstops (in multiples of the space width), default is 8 **/
        void SetTabWidth(unsigned int width) {mTabWidth = width;}

        const unsigned int GetTabWidth() const {return mTabWidth;}

    protected:
    private:

        FontData mFontData;
        sf::Image* mImage; //pointer so I can get away with a forward declaration
        unsigned int mTabWidth; ///< In multiples of space width, default 8
};

} // namespace jar

#endif // JAR_GRAPHICS_FONT_HPP
