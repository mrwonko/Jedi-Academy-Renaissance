#ifndef JAR_LUABIND_CORE_HPP
#define JAR_LUABIND_CORE_HPP

struct lua_State;
#include <jar/core/API.hpp>

namespace jar
{

/** \brief Binds core classes to Lua via luabind, like the Log.
**/
void JARCOREAPI BindCore(lua_State* L);
}

#endif
