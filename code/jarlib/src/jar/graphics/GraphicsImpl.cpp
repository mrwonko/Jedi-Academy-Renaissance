#include "jar/Core.hpp"
#include "jar/core/Lua.hpp"
#include "jar/core/Logger.hpp"
#include "jar/graphics/GraphicsImpl.hpp"
#include "jar/graphics/RenderWindow.hpp"
#include "jar/graphics/Font.hpp"
#include "jar/graphics/Text.hpp"
#include "jar/graphics/g2Model.hpp"
#include "jar/graphics/TextureManager.hpp"
#include "jar/luabind/SFMLGraphics.hpp"

namespace jar {

GraphicsImpl::GraphicsImpl()
{
    //ctor
}

GraphicsImpl::~GraphicsImpl()
{
    //dtor
}

const bool GraphicsImpl::Init()
{
    Logger::GetDefaultLogger().Info("- Initializing Graphics component...", 1);
    lua_State* L = Core::GetSingleton().GetLua().GetState();
    BindSFMLGraphics(L);
    RenderWindow::Luabind(L);
    Font::BindToLua(L);
    Text::BindToLua(L);
    g2::Model::BindToLua(L);
    TextureManagerLuabind::BindToLua(L);
    Logger::GetDefaultLogger().Info("Exposed Graphics System to Lua", 2);
    Logger::GetDefaultLogger().Info("- Graphics component successfully initialized.", 1);
    return true;
}

const bool GraphicsImpl::Deinit()
{
    return true;
}

} // namespace jar
