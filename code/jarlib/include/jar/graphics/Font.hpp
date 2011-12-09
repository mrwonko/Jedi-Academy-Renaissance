#ifndef JAR_GRAPHICS_FONT_HPP
#define JAR_GRAPHICS_FONT_HPP

#include "jar/graphics/API.hpp"
#include <string>

struct lua_State;

namespace sf
{
    class Texture;
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
        const bool LoadFromFile(const std::string& filename, std::string& error);

        /** \brief Returns the width of a given string, in pixels, if it was written using this font.
        **/

        const unsigned int GetWidth(const std::string& str) const;

        static void BindToLua(lua_State* L);

        // from the JKA SDK
        // these structs corresponds 1:1 with the "*.fontdat" files, so don't change them
        struct GlyphInfo
        {
            GlyphInfo() : mWidth(0), mHeight(0), mHorizAdvance(0), mHorizOffset(0), mBaseline(0), mTexCoordX1(0.f), mTexCoordY1(0.f), mTexCoordX2(0.f), mTexCoordY2(0.f) {}
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
            FontData() : mPointSize(0), mHeight(0), mAscender(0), mDescender(0), mKoreanHack(0) {}
            static const unsigned int GLYPH_COUNT = 256;
            GlyphInfo	mGlyphs[GLYPH_COUNT];

            short		mPointSize;             //size of font in points
            short		mHeight;				// max height of font
            short		mAscender;
            short		mDescender;

            short		mKoreanHack;
        };

        const FontData& GetFontData() const { return mFontData; }

        const sf::Texture& GetTexture() const { return *mTexture; }

        /** \brief Sets the width of tabstops (in multiples of the space width), default is 8 **/
        void SetTabWidth(unsigned int width) {mTabWidth = width;}

        const unsigned int GetTabWidth() const {return mTabWidth;}

    protected:
    private:

        FontData mFontData;
        sf::Texture* mTexture; //pointer so I can get away with a forward declaration
        unsigned int mTabWidth; ///< In multiples of space width, default 8
};

} // namespace jar

#endif // JAR_GRAPHICS_FONT_HPP
