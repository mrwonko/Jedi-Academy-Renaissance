#ifndef JAR_GRAPHICS_TEXTURE_MANAGER_HPP
#define JAR_GRAPHICS_TEXTURE_MANAGER_HPP

#include <string>
#include <luabind/wrapper_base.hpp>
#include <luabind/object.hpp>

namespace sf
{
    class Texture;
}

struct lua_State;

namespace jar
{

    class TextureManager
    {
    public:

        /** \brief Retrieves the given texture, possibly loading it, and increases its use count.
            \return The texture, or an default texture if it's unavailable.
        **/
        virtual sf::Texture* AcquireTexture(const std::string& filename) = 0;

        /** \brief Decreases the reference count by one, possibly deleting the texture.
        **/
        virtual void ReleaseTexture(sf::Texture* tex) = 0;

    protected:
        
        TextureManager();
        virtual ~TextureManager();

    };

    class TextureManagerLuabind : public TextureManager
    {
    public:
        //TextureManagerLuabind();
        TextureManagerLuabind(lua_State* L);
        virtual ~TextureManagerLuabind();

        void SetLuaState(lua_State* L);

        static void BindToLua(lua_State* L);
        
        virtual sf::Texture* AcquireTexture(const std::string& filename);
        virtual void ReleaseTexture(sf::Texture* tex);

        /** \brief Returns a lua object with userdata of the texture
        **/
        virtual luabind::object GetTexture(lua_State* L, const std::string& filename);

    private:
        struct TextureHandle
        {
            TextureHandle(const luabind::object& obj);
            unsigned int numUsers;
            luabind::object handle; // keeps the object from being garbage collected
        };
        typedef std::map<sf::Texture*, TextureHandle> HandleMap;
        HandleMap mHandles;
        lua_State* mState;
    };

    /** \brief Helper class to make TextureManager Luabind-Inheritable
    **/
    class TextureManagerLuabindWrapper : public TextureManagerLuabind, public luabind::wrap_base
    {
    public:
        TextureManagerLuabindWrapper(lua_State* L);
        virtual ~TextureManagerLuabindWrapper();

        virtual luabind::object GetTexture(lua_State* L, const std::string& filename);
        static luabind::object GetTextureDefault(TextureManagerLuabind* mgr, lua_State* L, const std::string& filename);

    };

}

#endif