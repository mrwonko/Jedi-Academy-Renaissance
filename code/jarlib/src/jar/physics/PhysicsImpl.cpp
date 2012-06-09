#include "jar/physics/PhysicsImpl.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Lua.hpp"
#include "jar/Core.hpp"
namespace jar {

PhysicsImpl::PhysicsImpl()
{
    //ctor
}

PhysicsImpl::~PhysicsImpl()
{
    //dtor
}

const bool PhysicsImpl::Init()
{
    Logger::GetDefaultLogger().Info("- Initializing Physics component...", 1);

    //Logger::GetDefaultLogger().Info("Exposing Physics System to Lua...", 2);
    //BindPhysics(Core::GetSingleton().GetLua().GetState());

    Logger::GetDefaultLogger().Info("- Physics component initialized...", 1);

    return true;
}

const bool PhysicsImpl::Deinit()
{
    return true;
}

} // namespace jar
