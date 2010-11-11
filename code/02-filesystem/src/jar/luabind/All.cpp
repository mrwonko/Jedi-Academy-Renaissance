#include "jar/luabind/All.hpp"
#include "jar/luabind/STL.hpp"
#include "jar/luabind/FileSystem.hpp"
#include "jar/luabind/Core.hpp"

namespace jar
{

void BindAll(lua_State* L)
{
    BindCore(L);
    BindFileSystem(L);
    BindSTL(L);
}

}
