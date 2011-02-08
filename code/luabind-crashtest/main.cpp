//A simple program reproducing a crash I experienced earlier.
// - Willi "Mr. Wonko" Schinmeyer

#include <iostream>
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/wrapper_base.hpp>
#include <cassert>
#include <string>
extern "C"
{
#include <lauxlib.h>
}

//I inherit from this class in Lua.
struct myClass
{
    //This is used to call the virtual function from C++
    void CallVirtual(const std::string& foo) { Virtual(foo); }
    //Calling this method directly from Lua works. The parameter isn't necessary to reproduce the crash, it just helps with debug output.
    virtual void Virtual(const std::string& foo) = 0;
};

//Lua wrapper class
struct myClassLua : myClass, luabind::wrap_base
{
    void Virtual(const std::string& foo) { call<void>("Virtual", foo); }
    static void StaticVirtual(myClass& mc, const std::string& foo) { mc.Virtual(foo); }
};

int main()
{
    //initialize
    lua_State* L = lua_open();
    assert(L);
    luaL_openlibs(L);
    luabind::open(L);

    //export via luabind
    luabind::module(L)
    [
        luabind::class_<myClass, myClassLua>("myClass")
            .def(luabind::constructor<>())
            .def("Virtual", &myClass::Virtual, &myClassLua::StaticVirtual)
            .def("CallVirtual", &myClass::CallVirtual)
    ];

    //the lua code
    std::string str = "\
class 'myClassLua' (myClass) \n\
\n\
function myClassLua:__init() \n\
   myClass.__init(self) \n\
end \n\
\n\
function myClassLua:Virtual(foo) \n\
    local tab = {} \n\
    print(foo) \n\
    table.insert(tab, 1) \n\
    print('table.insert called.') \n\
end \n\
\n\
local obj = myClassLua() \n\
obj:Virtual('Works') -- works \n\
obj:CallVirtual('Crashes') -- crashes \n\
print('Didn\\'t crash?') \n\
    ";

    //execute the lua code
    if(luaL_loadbuffer(L, str.c_str(), str.size(), "string")||lua_pcall(L, 0, 0, 0))
    {
        //return != 0 means error.
        //the error is pushed onto the stack.
        std::cout<<lua_tostring(L, -1)<<std::endl;
    }

    return 0;
}
