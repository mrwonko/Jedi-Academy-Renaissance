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

//STL
#include <iostream>
#include <string>

//jar
#include <jar/Core.hpp>
#include <jar/core/CoutLogger.hpp>
#include <jar/core/FileSystem.hpp>
#include <jar/core/Lua.hpp>


int main(int argc, char** argv)
{
    //create Logger
    jar::CoutLogger logger;
    logger.IncreaseLoggingLevel(); //TODO: delete this or it make optional using -v?
    logger.Info("Initialized Logger", 1);

    //create + init core system (Filesystem, Lua, ...)
    jar::Core core;
    if(not core.Init(argc, argv, "../"))
    {
        return EXIT_SUCCESS;
    }

    if(not jar::fs::Mount("bootstrap.pk3", false))
    {
        logger.Error("Could not mount bootstrap.pk3: " + jar::fs::GetLastError());
        core.Deinit();
        return EXIT_SUCCESS;
    }
    logger.Info("Mounted bootstrap.pk3", 1);

    //main loop etc. is in Lua.
    std::string luaMainCode;
    if(not jar::fs::ReadFile("code/Bootstrap.lua", luaMainCode))
    {
        logger.Error("Could not read bootstrap.pk3/code/Bootstrap.lua:" + jar::fs::GetLastError());
        core.Deinit();
        return EXIT_SUCCESS;
    }
    logger.Info("Loaded code/Bootstrap.lua, now executing...", 1);

    if(!core.GetLua().ExecuteString(luaMainCode, "@code/Bootstrap.lua"))
    {
        logger.Error(core.GetLua().GetLastError());
    }
    logger.Info("Done!", 1);

    core.Deinit();
    return EXIT_SUCCESS;
}
