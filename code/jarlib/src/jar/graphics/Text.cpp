#include "jar/graphics/Text.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/graphics/Font.hpp"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
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
    UpdateCache();
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

void Text::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    float scaleFactor = mFontSize / mFont->GetFontData().mPointSize;
    //TODO/FIXME: order correct?
    states.transform *= getTransform();
    states.transform.scale(scaleFactor, scaleFactor);

    states.texture = &mFont->GetTexture();
    states.blendMode = sf::BlendAlpha;
    target.draw(mVertices.data(), mVertices.size(), sf::Quads, states);
}

void Text::UpdateCache()
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

    //clear cache
    mVertices.clear();

    //trivial case
    if(mText == "") return;

    const Font::FontData& data = mFont->GetFontData();

    const sf::Color* currentColor = &color_table[0];

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
            currentColor = &color_table[col];
            continue;
        }
        const Font::GlyphInfo& info = data.mGlyphs[static_cast<unsigned char>(*it)];

        int x = posX + info.mHorizOffset;
        int y = posY + data.mHeight - info.mBaseline;

        //left, top
        mVertices.push_back(sf::Vertex(sf::Vector2f(x, y + info.mHeight), *currentColor, sf::Vector2f(info.mTexCoordX1, info.mTexCoordY2)));
        //left, bottom
        mVertices.push_back(sf::Vertex(sf::Vector2f(x, y), *currentColor, sf::Vector2f(info.mTexCoordX1, info.mTexCoordY1)));
        //right, bottom
        mVertices.push_back(sf::Vertex(sf::Vector2f(x + info.mWidth, y), *currentColor, sf::Vector2f(info.mTexCoordX2, info.mTexCoordY1)));
        //right, top
        mVertices.push_back(sf::Vertex(sf::Vector2f(x + info.mWidth, y + info.mHeight), *currentColor, sf::Vector2f(info.mTexCoordX2, info.mTexCoordY2)));

        posX += info.mHorizAdvance;
    }
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
