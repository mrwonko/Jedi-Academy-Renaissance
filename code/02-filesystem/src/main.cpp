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
#include <jar/core/PhysFSFile.hpp> //TODO: delete once I've got the constant(s) elsewhere
#include <jar/core/CLArguments.hpp>
#include <jar/core/CoutLogger.hpp>
#include <jar/core/FileSystem.hpp>
#include <jar/core/Helpers.hpp>
#include <jar/core/Lua.hpp>
#include <jar/luabind/All.hpp>

#include <luabind/luabind.hpp> //TODO: delete once I no longer set a lua constant (DEBUG_ibiname) in main()

//physfs
#include <physfs.h>

/*
//minizip
#include <unzip.h>
int GetFileContent(unzFile filehandle, std::string& output)
{
    char buf[64];
    int status;
    while((status = unzReadCurrentFile(filehandle, buf, sizeof(buf))) > 0)
    {
        //status returns number of bytes read
        output += std::string(buf, status);
    }
    return status;
}
*/

int main(int argc, char** argv)
{
    try
    {
        //init Command Line Arguments
        jar::CLArguments args(argc, argv);
        //init Logger
        jar::CoutLogger logger;
        //TODO: delete / -v ?
        logger.IncreaseLoggingLevel();
        logger.Info("Initialized Logger", 1);
        //init Lua
        jar::Lua lua;
        if(!lua.Init())
        {
            logger.Error("Could not init Lua: "+lua.GetLastError());
            return 0;
        }
        if(!lua.OpenBaseLibrary() || !lua.OpenMathLibrary() || !lua.OpenStringLibrary() || !lua.OpenTableLibrary() || !lua.OpenPackageLibrary())
        if(!lua.Init())
        {
            logger.Error("Could not open Lua libraries: "+lua.GetLastError());
            return 0;
        }
        jar::BindFileSystem(lua.GetState());
        jar::BindCore(lua.GetState());
        logger.Info("Initialized Lua", 1);

        //For now: ask user for desired ibi filename
        std::string filename;
        std::cout<<"What ibi file shall be converted? "<<std::flush;
        std::cin>>filename;
        std::cin.clear();
        luabind::globals(lua.GetState())["DEBUG_ibiname"] = filename;

        //init physicsFS
        assert(argc > 0);
        if(!PHYSFS_init(argv[0]))
        {
            logger.Error(std::string(PHYSFS_getLastError()));
            return 0;
        }
        PHYSFS_setWriteDir((args.GetWorkingDirectory() + jar::MODDIR).c_str());
        logger.Info("Initialized PhysicsFS", 1);

        if(!PHYSFS_mount((args.GetWorkingDirectory() + jar::MODDIR + "bootstrap.pk3").c_str(), NULL, 0))
        {
            logger.Error("Could not mount bootstrap.pk3: " + std::string(PHYSFS_getLastError()));
            PHYSFS_deinit();
            lua.Deinit();
            return 0;
        }
        logger.Info("Mounted bootstrap.pk3", 1);

        PHYSFS_File* file = PHYSFS_openRead("code/Bootstrap.lua");
        if(file == NULL)
        {
            logger.Error("Could not load code/Bootstrap.lua: " + std::string(PHYSFS_getLastError()));
            PHYSFS_deinit();
            lua.Deinit();
            return 0;
        }

        //main loop etc. is in Lua.
        std::string luaMainCode;
        //own scope so buf & status get deleted before main loop
        {
            char buf[1024];
            int status;
            while((status = PHYSFS_read(file, buf, sizeof(char), sizeof(buf))) > 0)
            {
                luaMainCode += std::string(buf, status);
            }
            PHYSFS_close(file);
            if(status == -1)
            {
                logger.Error(std::string(PHYSFS_getLastError()));
                PHYSFS_deinit();
                lua.Deinit();
                return 0;
            }
        }
        logger.Info("Loaded code/Bootstrap.lua, executing...", 1);
        if(!lua.ExecuteString(luaMainCode, "@code/Bootstrap.lua"))
        {
            logger.Error(lua.GetLastError());
        }
        logger.Info("Done, core systems down.", 1);

        //deinit PhysicsFS
        if(!PHYSFS_deinit())
        {
            //now PhysicsFS is "probably badly screwed up".
            logger.Error(std::string(PHYSFS_getLastError()));
            return 0;
        }
        lua.Deinit();
    }
    catch(std::exception& e)
    {
        std::cout<<"Exception: "<<e.what()<<std::endl;
    }

    std::cout<<"Done. Enter text and press enter to exit."<<std::endl;
    char dummy;
    std::cin>>dummy;

    return 0;
}
