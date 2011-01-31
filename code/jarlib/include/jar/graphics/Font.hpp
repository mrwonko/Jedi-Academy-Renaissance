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

    protected:
    private:

        FontData mFontData;
        sf::Image* mImage; //pointer so I can get away with a forward declaration
};

} // namespace jar

#endif // JAR_GRAPHICS_FONT_HPP
