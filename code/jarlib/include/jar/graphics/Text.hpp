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
        virtual ~Text();

        /** \brief Sets the font, does not change the size **/
        void SetFont(const Font& font);

        /** \brief Gets the font **/
        const Font& GetFont();

        /** \brief Sets the text **/
        void SetText(const std::string& text);

        /** \brief Gets the text **/
        const std::string& GetText();

        /** \brief Sets the font size, in points **/
        void SetFontSize(const float size);

        /** \brief Gets the font size, in points (default: font's size) **/
        const float GetFontSize();

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
