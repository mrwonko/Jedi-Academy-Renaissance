extern "C"
{
    #include "lauxlib.h"
    #include "lualib.h"
}

#include <iostream>
#include <luabind/luabind.hpp>
#include <luabind/class_info.hpp>

int pcall_handler(lua_State* /*L*/)
{
	return 1;
}

void dostring(lua_State* state, char const* str)
{
    lua_pushcclosure(state, &pcall_handler, 0);

    if (luaL_loadbuffer(state, str, std::strlen(str), str))
    {
        std::string err("Error loading buffer: ");
        err += lua_tostring(state, -1);
        lua_pop(state, 2);
		throw err;
    }

    if (lua_pcall(state, 0, 0, -2))
    {
        std::string err("Error executing: ");
        err += lua_tostring(state, -1);
        lua_pop(state, 2);
		throw err;
    }

    lua_pop(state, 1);
}

void HelloWorld()
{
    std::cout<<"Hello World!"<<std::endl;
}

void HelloWorld(const std::string& text)
{
    std::cout<<"Hello World! "<<text<<std::endl;
}

struct MyClass
{
    void Hello()
    {
        std::cout<<"My Class says Hello!"<<std::endl;
    }
};

int main()
{
    lua_State* L = lua_open();
    luaopen_base(L);
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
	 // lua 5.1 or newer
	 luaL_openlibs(L);
#else
	 // lua 5.0.2 or older
    lua_baselibopen(L);
#endif
    luabind::open(L);

    luabind::bind_class_info(L);

    luabind::module(L, "namespace")
    [
        luabind::def("HelloWorld", (void(*)())&HelloWorld),
        luabind::def("HelloWorld", (void(*)(const std::string&))&HelloWorld),
        luabind::class_<MyClass>("MyClass")
            .def(luabind::constructor<>())
            .def("Hello", &MyClass::Hello)
    ];

    try
    {
        dostring(L,
//                 "success, msg = pcall(function()\n"

                 "namespace.HelloWorld(1)\n" // I don't get the correct error when using the wrong function signature - problem with this luabind fork?
                 "testUs = class_names()\n"
                 "table.insert(testUs, \"int\") int = 1\n"
                 "table.insert(testUs, \"str\") str = \"test\"\n"
                 "table.insert(testUs, \"print\")\n"
                 "table.insert(testUs, \"table\")\n"
                 "for _, name in ipairs(testUs) do\n"
                 " class = _G[name]\n"
                 " if not class then\n"
                 "  print(\"could not find class \" .. name)\n"
                 "  print()\n"
                 " else\n"
                 "  info = class_info(class)\n"
                 "  print(\"== \" .. info.name .. \" ==\")\n"
                 "  print(\"= attributes =\")\n"
                 "  for _, attrib in ipairs(info.attributes) do\n"
                 "   print(attrib)\n"
                 "  end\n"
                 "  print(\"= methods =\")\n"
                 "  for func, _ in pairs(info.methods) do\n"
                 "   print(func)\n"
                 "  end\n"
                 "  print()\n"
                 " end\n"
                 "end\n"

//                 "end)\n"
//                 "if not success then\n"
//                 " print(msg)\n"
//                 "end\n"
                 );
    }
    catch(std::string err)
    {
        std::cout<<"Caught error: " << err << std::endl;
    }

    lua_close(L);
    return 0;
}
