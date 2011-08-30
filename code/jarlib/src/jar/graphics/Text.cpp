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

#include "jar/graphics/Text.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/graphics/Font.hpp"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Renderer.hpp>
#include <cassert>

namespace jar {

Text::Text() :
    mFont(NULL),
    mFontSize(12)
{
    //ctor
}

Text::Text(const Font& font) :
    mFont(&font),
    mFontSize(font.GetFontData().mPointSize)
{
    //ctor
}
Text::Text(const Font& font, const unsigned int fontSize) :
    mFont(&font),
    mFontSize(fontSize)
{
    //ctor
}

Text::~Text()
{
    //dtor
}

void Text::SetText(const std::string& text)
{
    mText = text;
}

const std::string& Text::GetText() const
{
    return mText;
}

void Text::SetFont(const Font& font)
{
    mFont = &font;
}

const Font& Text::GetFont() const
{
    assert(mFont);
    return *mFont;
}

void Text::SetFontSize(const float size)
{
    mFontSize = size;
}

const float Text::GetFontSize() const
{
    return mFontSize;
}

void Text::Render(sf::RenderTarget& target, sf::Renderer& renderer) const
{
    static sf::Color color_table[8] =
    {
        sf::Color::Black, //0
        sf::Color::Red, //1
        sf::Color::Green, //2
        sf::Color::Yellow, //3
        sf::Color::Blue, //4
        sf::Color::Cyan, //5
        sf::Color::Magenta, //6
        sf::Color::White, //7
    };

    //trivial case
    if(mText == "") return;

    const Font::FontData& data = mFont->GetFontData();

    //NOTE: I don't know how to do this using renderer, so I'm multiplying it. If this turns out to slow stuff down, I might want to change it.
    float scaleFactor = mFontSize / data.mPointSize;

    //render code goes here
    renderer.SetTexture(&mFont->GetTexture());
    renderer.SetBlendMode(sf::Blend::Alpha);
    renderer.Begin(sf::Renderer::QuadList);

    int posX = 0;
    int posY = -data.mDescender;
    bool nextIsColor = false;

    int tabWidth = data.mGlyphs[' '].mHorizAdvance * mFont->GetTabWidth();

    for(std::string::const_iterator it = mText.begin(); it != mText.end(); ++it)
    {
        if(*it == '\n')
        {
            posX = 0;
            posY += data.mHeight;
            continue;
        }
        if(*it == '\t')
        {
            posX = posX - posX%tabWidth + tabWidth;
            continue;
        }
        if(*it == '^')
        {
            std::string::const_iterator next = it + 1;
            if(next != mText.end() && *next >= '0' && *next <= '7')
            {
                nextIsColor = true;
                continue;
            }
        }
        if(nextIsColor)
        {
            nextIsColor = false;
            assert(*it >= '0' && *it <= '7');
            unsigned int col = *it - '0';
            renderer.ApplyColor(color_table[col]);
            continue;
        }
        const Font::GlyphInfo& info = data.mGlyphs[static_cast<unsigned char>(*it)];

        int x = posX + info.mHorizOffset;
        int y = posY + data.mHeight - info.mBaseline;

        //left, top
        renderer.AddVertex(scaleFactor*x, scaleFactor*(y + info.mHeight), info.mTexCoordX1, info.mTexCoordY2);
        //left, bottom
        renderer.AddVertex(scaleFactor*x, scaleFactor*y, info.mTexCoordX1, info.mTexCoordY1);
        //right, bottom
        renderer.AddVertex(scaleFactor*(x + info.mWidth), scaleFactor*y, info.mTexCoordX2, info.mTexCoordY1);
        //right, top
        renderer.AddVertex(scaleFactor*(x + info.mWidth), scaleFactor*(y + info.mHeight), info.mTexCoordX2, info.mTexCoordY2);

        posX += info.mHorizAdvance;
    }
    renderer.End();
}

const float Text::GetWidth() const
{
    return (float)mFontSize / (float) mFont->GetFontData().mPointSize * (float) mFont->GetWidth(mText);
}

const float Text::GetLineHeight() const
{
    return (float)mFontSize/(float)mFont->GetFontData().mPointSize * mFont->GetFontData().mHeight;
}

const float Text::GetHeight() const
{
    int lines = 1;
    for(std::string::const_iterator it = mText.begin(); it != mText.end(); ++it)
    {
        if(*it == '\n') ++lines;
    }
    return lines * GetLineHeight();
}

} // namespace jar
