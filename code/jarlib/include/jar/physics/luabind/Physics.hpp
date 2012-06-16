#ifndef JAR_PHYSICS_LUABIND_PHYSICS_HPP
#define JAR_PHYSICS_LUABIND_PHYSICS_HPP

struct lua_State;

namespace jar
{

void BindPhysicsToLua(lua_State* L);

}

#endif
