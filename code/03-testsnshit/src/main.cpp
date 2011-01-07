/*
===========================================================================
Copyright (C) 2010 Willi Schinmeyer

This file is part of the Jedi Academy: Renaissance source code.

Jedi Academy: Renaissance source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Jedi Academy: Renaissance source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jedi Academy: Renaissance source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include <jar/Core.hpp>
#include <jar/Input.hpp>
#include <jar/Graphics.hpp>
#include <jar/Core/CoutLogger.hpp>
#include <jar/Core/FileSystem.hpp>
#include <jar/Core/Lua.hpp>


//LUABIND TESTS
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/object.hpp>
#include <luabind/class_info.hpp>

struct luabind_test
{
    luabind_test() : mNum(0) {}

    int Double(int num) { return 2*num; }

    void Print() { jar::Logger::GetDefaultLogger().Log("Hello World!"); }

    int GetNum() { return mNum; }

    void SetNum(const int num) { mNum = num; }

    std::string GetClass() { return "luabind_test"; }

    private:
        int mNum;
};

static luabind_test* GetLuabindTest()
{
    luabind_test* obj = new luabind_test;
    obj->SetNum(42);
    return obj;
}


static void LuabindInfo(luabind::object o)
{
    if(!o.is_valid())
    {
        jar::Logger::GetDefaultLogger().Warning("Invalid object passed to LuabindInfo()!");
        return;
    }

    int formerStacksize = lua_gettop(o.interpreter());

    o.push(o.interpreter());

    luabind::class_info info = luabind::get_class_info(luabind::from_stack(o.interpreter(), -1));

    jar::Logger::GetDefaultLogger().Log("Class: \"" + info.name + "\"");

    lua_pop(o.interpreter(), 1);
    assert(lua_gettop(o.interpreter()) == formerStacksize);
}

int main(int argc, char** argv)
{
    static const bool RELEASE = false;

    //create Logger
    jar::CoutLogger logger;
    logger.SetLoggingLevel(5);
    logger.Info("Initialized Logger", 0);

    //initialize core & other components
    jar::Core core;
    jar::Input input; // just create this once (after core), no need to do anything else.
    jar::Graphics graphics; // just create this once (after core), no need to do anything else.

    if(!core.Init(argc, argv, "../"))
    {
        return 0;
    }

    luabind::module(core.GetLua().GetState(), "tests")
    [
        luabind::class_<luabind_test>("luabind_test")
            .def("Double", &luabind_test::Double)
            .def("Print", &luabind_test::Print)
            .def("GetNum", &luabind_test::GetNum)
            .def("SetNum", &luabind_test::SetNum)
            .def("GetClass", &luabind_test::GetClass),
        luabind::def("LuabindInfo", LuabindInfo),
        luabind::def("GetLuabindTest", GetLuabindTest)
    ];

    //TODO: when RELEASE, set RELEASE in Lua, too.

    if( (RELEASE && !jar::fs::Mount("bootstrap.pk3", false) ) || (!RELEASE && !jar::fs::Mount("./", false) ) )
    {
        logger.Error("Could not mount " + (RELEASE ? std::string("bootstrap.pk3") : std::string("./") ) +": " + jar::fs::GetLastError());
        core.Deinit();
        return EXIT_SUCCESS;
    }
    logger.Info((RELEASE ? "Mounted bootstrap.pk3" : "Mounted ./"), 0);

    logger.Log("Last Error: " + jar::fs::GetLastError());

    //main loop etc. is in Lua.
    std::string luaMainCode;
    if(!jar::fs::ReadFile("code/Bootstrap.lua", luaMainCode))
    {
        logger.Error("Could not read bootstrap.pk3/code/Bootstrap.lua:" + jar::fs::GetLastError());
        core.Deinit();
        return EXIT_SUCCESS;
    }
    logger.Log("Last Error: " + jar::fs::GetLastError());
    logger.Info("Loaded code/Bootstrap.lua, now executing...\n", 0);

    if(!core.GetLua().ExecuteString(luaMainCode, "@code/Bootstrap.lua"))
    {
        logger.Error(core.GetLua().GetLastError());
    }

    core.Deinit();

    return 0;
}
