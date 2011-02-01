#include "jar/graphics/Text.hpp"
#include "jar/graphics/Font.hpp"
#include <SFML/Graphics/Image.hpp>
#include <GL/gl.h>
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

Text::~Text()
{
    //dtor
}

void Text::SetText(const std::string& text)
{
    mText = text;
}

const std::string& Text::GetText()
{
    return mText;
}

void Text::SetFont(const Font& font)
{
    mFont = &font;
}

const Font& Text::GetFont()
{
    assert(mFont);
    return *mFont;
}

void Text::SetFontSize(const float size)
{
    mFontSize = size;
}

const float Text::GetFontSize()
{
    return mFontSize;
}

void Text::Render(sf::RenderTarget& target) const
{
    static float color_table[8][4] =
    {
        {0.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 1.0, 0.0, 1.0},
        {0.0, 0.0, 1.0, 1.0},
        {0.0, 1.0, 1.0, 1.0},
        {1.0, 0.0, 1.0, 1.0},
        {1.0, 1.0, 1.0, 1.0},
    };

    //trivial case
    if(mText == "") return;

    const Font::FontData& data = mFont->GetFontData();

    //needs to be done before quad code
    {
        float scaleFactor = mFontSize / data.mPointSize;
        glScalef(scaleFactor, scaleFactor, 1.f);
    }

    //render code goes here
    mFont->GetImage().Bind();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);

    float posX = 0;
    float posY = 0;
    bool nextIsColor = false;

    for(std::string::const_iterator it = mText.begin(); it != mText.end(); ++it)
    {
        if(*it == '\n')
        {
            posX = 0;
            posY += data.mHeight;
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
            glColor4f(color_table[col][0], color_table[col][1], color_table[col][2], color_table[col][3]);
            continue;
        }
        const Font::GlyphInfo& info = data.mGlyphs[static_cast<unsigned char>(*it)];

        float x = posX + info.mHorizOffset;
        float y = posY + data.mAscender - info.mBaseline;

        //left, top
        glTexCoord2f(info.mTexCoordX1, info.mTexCoordY2);
        glVertex2f(x, y + info.mHeight);
        //left, bottom
        glTexCoord2f(info.mTexCoordX1, info.mTexCoordY1);
        glVertex2f(x, y);
        //right, bottom
        glTexCoord2f(info.mTexCoordX2, info.mTexCoordY1);
        glVertex2f(x + info.mWidth, y);
        //right, top
        glTexCoord2f(info.mTexCoordX2, info.mTexCoordY2);
        glVertex2f(x + info.mWidth, y + info.mHeight);

        posX += info.mHorizAdvance;
    }
    glEnd();
}

} // namespace jar
