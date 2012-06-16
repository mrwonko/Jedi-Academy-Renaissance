#include "jar/physics/PhysicsWorld.hpp"
#include "jar/physics/luabind/Physics.hpp"
#include <luabind/luabind.hpp>

namespace jar
{

void BindPhysicsToLua(lua_State* L)
{
    luabind::module(L, "jar")
        [
            luabind::class_<PhysicsWorld>("PhysicsWorld")
            .def(luabind::constructor<>())
        ];
}

}
