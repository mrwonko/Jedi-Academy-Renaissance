#include <jar/Core.hpp>
#include <jar/Input.hpp>
#include <jar/Graphics.hpp>
#include <jar/Audio.hpp>
#include <jar/Physics.hpp>
#include <jar/core/CoutAndFileLogger.hpp>
#include <jar/core/FileSystem.hpp>
#include <jar/core/Lua.hpp>

#include "SimpleLevel.hpp"

#include <iostream>
#include <exception>
#include <cstdlib> //EXIT_SUCCESS

int main(int argc, char** argv)
{
    std::cout<<"Go go go!"<<std::endl;
    try
    {
        static const bool RELEASE = false;
        
        //create Logger
        jar::CoutAndFileLogger logger("../log.txt");
        logger.SetLoggingLevel(5);
        logger.Info("Initialized Logger", 0);

        //initialize core & other components
        jar::Core core;
        jar::Input input; // just create this once (after core), no need to do anything else.
        jar::Graphics graphics; // just create this once (after core), no need to do anything else.
        jar::Audio audio; // just create this once (after core), no need to do anything else.
        jar::Physics physics; // just create this once (after core), no need to do anything else.

        if(!core.Init(argc, argv, "../"))
        {
            return 0;
        }

        SimpleLevel::BindToLua(core.GetLua().GetState());

        //TODO: when RELEASE, set RELEASE in Lua, too. (For proper mounting)

        if( (RELEASE && !jar::fs::Mount("bootstrap.pk3", false) ) || (!RELEASE && !jar::fs::Mount("./", false) ) )
        {
            logger.Error("Could not mount " + (RELEASE ? std::string("bootstrap.pk3") : std::string("./") ) +": " + jar::fs::GetLastError());
            core.Deinit();
            return EXIT_SUCCESS;
        }
        logger.Info((RELEASE ? "Mounted bootstrap.pk3" : "Mounted ./"), 0);

        //main loop etc. is in Lua.
        std::string luaMainCode;
        if(!jar::fs::ReadFile("code/Bootstrap.lua", luaMainCode))
        {
            logger.Error("Could not read bootstrap.pk3/code/Bootstrap.lua:" + jar::fs::GetLastError());
            core.Deinit();
            return EXIT_SUCCESS;
        }
        //logger.Log("Last Error: " + jar::fs::GetLastError());
        logger.Info("Loaded code/Bootstrap.lua, now executing...\n", 0);

        if(!core.GetLua().ExecuteString(luaMainCode, "@code/Bootstrap.lua"))
        {
            logger.Error(core.GetLua().GetLastError());
        }

        core.Deinit();
    }
    catch(std::exception& e)
    {
        std::cout<<"\ncaught exception: "<<e.what()<<std::endl;
    }
    catch(...)
    {
        std::cout<<"\ncaught exception!"<<std::endl;
    }
    return 0;
}
