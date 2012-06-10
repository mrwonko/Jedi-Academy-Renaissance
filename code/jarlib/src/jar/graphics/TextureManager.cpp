#include "jar/graphics/TextureManager.hpp"
#include "jar/core/Logger.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <cassert>

namespace jar
{
    TextureManager::TextureManager()
    {
    }

    TextureManager::~TextureManager()
    {
    }

    TextureManagerLuabind::TextureManagerLuabind(lua_State* L) :
        mState(L)
    {
    }

    TextureManagerLuabind::~TextureManagerLuabind()
    {
    }

    TextureManagerLuabind::TextureHandle::TextureHandle(const luabind::object& obj) :
        numUsers(1),
        handle(obj)
    {
    }

    void TextureManagerLuabind::SetLuaState(lua_State* L)
    {
        mState = L;
    }

    sf::Texture* TextureManagerLuabind::AcquireTexture(const std::string& filename)
    {
        if(mState == NULL)
        {
            Logger::GetDefaultLogger().Error("TextureManagerLuabind::AcquireTexture(): Not initialized!");
            return NULL;
        }
        luabind::object texture = GetTexture(mState, filename);
        if(luabind::type(texture) == LUA_TNIL)
        {
            return NULL;
        }
        sf::Texture* rawTexture = luabind::object_cast<sf::Texture*>(texture);
        assert(rawTexture != NULL); //or it would be nil, right?

        HandleMap::iterator pos = mHandles.find(rawTexture);
        if(pos == mHandles.end())
        {
            mHandles.insert(HandleMap::value_type(rawTexture, TextureHandle(texture))); // storing the object ensures it's not garbage collected (yay luabind!)
        }
        else
        {
            pos->second.numUsers += 1;
        }
        return rawTexture;
    }

    void TextureManagerLuabind::ReleaseTexture(sf::Texture* tex)
    {
        HandleMap::iterator pos = mHandles.find(tex);
        if(pos == mHandles.end())
        {
            if(tex != NULL)
            {
                Logger::GetDefaultLogger().Warning("TextureManagerLuabind::ReleaseTexture() called on unowned texture! This hints at a memory leak.");
            }
            return;
        }
        assert(pos->second.numUsers > 0);
        pos->second.numUsers -= 1;
        if(pos->second.numUsers == 0)
        {
            mHandles.erase(pos);
        }
    }

    luabind::object TextureManagerLuabind::GetTexture(lua_State* L, const std::string& filename)
    {
        // to be overwritten by Lua
        return luabind::object();
    }

    void TextureManagerLuabind::BindToLua(lua_State* L)
    {
        luabind::module(L, "jar")
        [
            luabind::class_<TextureManagerLuabind, TextureManagerLuabindWrapper>("TextureManager")
                //.def(luabind::constructor<>())
                .def(luabind::constructor<lua_State*>())
                .def("GetTexture", &TextureManagerLuabind::GetTexture, &TextureManagerLuabindWrapper::GetTextureDefault)
                //.def("Init", &TextureManagerLuabind::SetLuaState)
        ];
    }
    
    luabind::object TextureManagerLuabindWrapper::GetTexture(lua_State* L, const std::string& filename)
    {
        return call<luabind::object>("GetTexture", filename);
    }

    luabind::object TextureManagerLuabindWrapper::GetTextureDefault(TextureManagerLuabind* mgr, lua_State* L, const std::string& filename)
    {
        return mgr->GetTexture(L, filename);
    }

    TextureManagerLuabindWrapper::TextureManagerLuabindWrapper(lua_State* L) :
        TextureManagerLuabind(L)
    {
    }

    TextureManagerLuabindWrapper::~TextureManagerLuabindWrapper()
    {
    }
}