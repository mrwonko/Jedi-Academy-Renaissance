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

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <SFML/Graphics.hpp>
#include <GL/gl.h>

// declaration

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Image.hpp>
#include <string>

namespace sf
{
    class RenderTarget;
}

class Font : public sf::Drawable
{
public:
    Font();
    virtual ~Font();
    /** \param filename without extension - a *filename*.png and a *filename*.fontdat should exist.
    **/
    const bool LoadFromFile(const std::string& filename);

    virtual void Render(sf::RenderTarget& target) const;

    //usually a char is signed - that doesn't work in this case.
    unsigned char mRenderMe; //TODO: Delete

private:
    // from the JKA SDK
    // these structs corresponds 1:1 with the "*.fontdat" files, so don't change it
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

    FontData mFontData;
    sf::Image mImage;
};

// implementation

Font::Font() :
    mRenderMe('!')
{
    //ctor
}

Font::~Font()
{
    //dtor
}

const bool Font::LoadFromFile(const std::string& filename)
{
    //TODO: PhysFS
    std::ifstream stream((filename + ".fontdat").c_str(), std::ios_base::in | std::ios_base::binary);
    if(stream.fail())
    {
        //TODO: Logger
        std::cout<<"Could not load "<<filename<<".fontdat!"<<std::endl;
        return false;
    }
    stream.read(reinterpret_cast<char *>(&mFontData),sizeof(FontData));
    stream.close();

    //TODO: PhysFS
    if(!mImage.LoadFromFile(filename + ".png"))
    {
        //TODO: Logger
        std::cout<<"Could not load "<<filename<<".png!"<<std::endl;
        return false;
    }

    std::cout<<"Asc: "<<mFontData.mAscender << " Desc: " << mFontData.mDescender << " Height: " << mFontData.mHeight << std::endl;
    return true;
}

void Font::Render(sf::RenderTarget& target) const
{
    const GlyphInfo& info = mFontData.mGlyphs[mRenderMe];

    //render code goes here
    mImage.Bind();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);

/*
    static int lastBaseline = 0;
    if(info.mBaseline != lastBaseline)
    {
        lastBaseline = info.mBaseline;
        std::cout<<"Baseline " << mRenderMe << ": " << info.mBaseline<<std::endl;
    }
*/

    int xOfs = info.mHorizOffset;
    int yOfs = mFontData.mAscender - info.mBaseline;
    //left, top
    glTexCoord2f(info.mTexCoordX1, info.mTexCoordY2);
    glVertex2f(0 + xOfs, info.mHeight + yOfs);
    //left, bottom
    glTexCoord2f(info.mTexCoordX1, info.mTexCoordY1);
    glVertex2f(0 + xOfs, 0 + yOfs);
    //right, bottom
    glTexCoord2f(info.mTexCoordX2, info.mTexCoordY1);
    glVertex2f(info.mWidth + xOfs, 0 + yOfs);
    //right, top
    glTexCoord2f(info.mTexCoordX2, info.mTexCoordY2);
    glVertex2f(info.mWidth + xOfs, info.mHeight + yOfs);
    glEnd();
}

int main(int argc, char** argv)
{
    std::cout<<"ATTENTION! There are BUGS in teh Boba'z coad!\n\
The actual mAscender is mHeight + your mDescender, while the actual mDescender is -your mDescender!\n\
And the image you output is white with alpha, but it should be black'n'white!"<<std::endl;

    std::string fonts[] =
    {
        "arial",
        "impact",
        "lucida",
        "times",
        "verdana",
        "anewhope"
    };

    //  Font Choosing

    unsigned int numfonts = sizeof(fonts) / sizeof(fonts[0]);
    std::cout<<"Which font would you like to use?"<<std::endl;
    for(unsigned int i = 0; i < numfonts; ++i)
    {
        std::cout<<i<<" - " << fonts[i]<<std::endl;
    }
    unsigned int font = 0;
    do
    {
        std::cout<<"Please make a valid choice: "<<std::flush;
        std::cin>>font;
    }
    while(font >= numfonts);
    std::cout<<"You chose " << fonts[font]<<"."<<std::endl;

    //  Font Data display

    Font f;
    if(!f.LoadFromFile("fonts/" + fonts[font]))
    {
        return 0;
    }

    //  Test Rendering
    sf::RenderWindow App(sf::VideoMode(64, 64), "Font Test Shit");
    while(App.IsOpened())
    {
        sf::Event e;
        while(App.GetEvent(e))
        {
            if(e.Type == sf::Event::Closed)
            {
                App.Close();
            }
            if(e.Type == sf::Event::KeyPressed)
            {
                if(e.Key.Code == sf::Key::Escape)
                {
                    App.Close();
                }
                else if(e.Key.Code == sf::Key::Space || (e.Key.Code >= 'a' && e.Key.Code <= 'z'))
                {
                    f.mRenderMe = (char)e.Key.Code + (e.Key.Shift ? 'A' - 'a' : 0);
                }
                else
                {
                    f.mRenderMe = 'ö';
                }
            }
        }
        App.Clear(sf::Color::Blue);
        App.Draw(f);
        App.Display();
    }
    return 0;
}
