#ifndef JAR_LUABIND_SFMLGRAPHICS_HPP
#define JAR_LUABIND_SFMLGRAPHICS_HPP

struct lua_State;
#include <jar/graphics/API.hpp>
#include <string>

namespace sf
{
    class Texture;
}

namespace jar
{

/** \brief Binds some SFML/Graphics classes to Lua via luabind, like Drawable and Color.
**/
void JARGRAPHICSAPI BindSFMLGraphics(lua_State* L);

/** \brief Loads an image from a file using PhysicsFS
**/
const bool JARGRAPHICSAPI TextureLoadFromFile(sf::Texture& image, const std::string& filename);
}

#endif
