#include "jar/graphics/Font.hpp"
#include "jar/graphics/luabind/SFMLGraphics.hpp" //contains TextureLoadFromFile()
#include "jar/core/Logger.hpp"
#include "jar/core/FileSystem.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <algorithm>
#include <cassert>

namespace jar {

Font::Font() :
    mTexture(new sf::Texture()),
    mTabWidth(8)
{
    //ctor
    assert(mTexture && "Could not allocate memory!");
	mTexture->setRepeated(true);
}

Font::~Font()
{
}

const bool Font::LoadFromFile(const std::string& filename, std::string& error)
{
    PHYSFS_File* file = fs::OpenRead(filename + ".fontdat");
    if(!file)
    {
        error = ("Could not load "+filename+".fontdat!");
        return false;
    }
    int numRead = fs::Read(file, &mFontData, sizeof(FontData), 1);
    if(numRead != 1)
    {
        if(fs::EndOfFile(file))
        {
            error = ("Unexcepted end of file "+filename+".fontdat!");
            fs::Close(file);
            return false;
        }
        error = ("Could not read "+filename+".fontdat: " + fs::GetLastError());
        fs::Close(file);
        return false;
    }
    fs::Close(file);

    if(!TextureLoadFromFile(*mTexture, filename))
    {
        error = ("Could not load image " + filename + "!");
        return false;
    }

    return true;
}

const unsigned int Font::GetWidth(const std::string& str) const
{
    unsigned long width = 0;
    unsigned long curWidth = 0;
    unsigned int tabWidth = mFontData.mGlyphs[' '].mHorizAdvance * mTabWidth;
	std::string::const_iterator end = str.end();
    for(std::string::const_iterator it = str.begin(); it != end; ++it)
    {
        if(*it == '\n')
        {
            width = std::max(width, curWidth);
            curWidth = 0;
            continue;
        }
        if(*it == '\t')
        {
            curWidth = curWidth - curWidth%tabWidth + tabWidth;
            continue;
        }
        //don't count color setters!
        if(*it == '^')
        {
            std::string::const_iterator next = it + 1;
            if(next != end && (*next >= '0') && (*next <= '7'))
            {
                ++it; //skip this and the next one
                //cppcheck false positive here - it cannot be incremented past the end because next already checks for ==str.end()
                continue;
            }
        }
        const GlyphInfo& info = mFontData.mGlyphs[static_cast<unsigned char>(*it)];
        curWidth += info.mHorizAdvance;
    }
    width = std::max(width, curWidth);
    return width;
}

} // namespace jar
