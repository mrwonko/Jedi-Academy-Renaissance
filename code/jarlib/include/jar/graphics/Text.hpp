#ifndef JAR_GRAPHICS_TEXT_HPP
#define JAR_GRAPHICS_TEXT_HPP

#include "jar/graphics/API.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <string>
#include <vector>

struct lua_State;

namespace jar {

class Font;

class JARGRAPHICSAPI Text : public sf::Drawable, public sf::Transformable
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

        /** \brief Gets the height of a single line - depends on font and fontsize **/
        const float GetLineHeight() const;

        /** \brief Gets the height of the text, in pixels **/
        const float GetHeight() const;

        virtual void Render(sf::RenderTarget& target, sf::RenderStates states) const;

        static void BindToLua(lua_State* L);
    protected:
    private:
        std::string mText;
        const Font* mFont;
        float mFontSize;

        std::vector<sf::Vertex> mVertices;
        void UpdateCache();
};

} // namespace jar

#endif // JAR_GRAPHICS_TEXT_HPP
