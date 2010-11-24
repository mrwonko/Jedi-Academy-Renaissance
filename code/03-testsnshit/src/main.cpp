#include <jar/Core.hpp>
#include <jar/Input.hpp>
#include <jar/Core/CoutLogger.hpp>
#include <jar/Core/FileSystem.hpp>
#include <jar/Core/Lua.hpp>

int main(int argc, char** argv)
{
    //create Logger
    jar::CoutLogger logger;
    logger.Info("Initialized Logger", 0);

    //initialize core & other components
    jar::Core core;
    jar::Input input; // just create this once (after core), no need to do anything else.

    if(!core.Init(argc, argv, "../"))
    {
        return 0;
    }

    if(!jar::fs::Mount("bootstrap.pk3", false))
    {
        logger.Error("Could not mount bootstrap.pk3: " + jar::fs::GetLastError());
        core.Deinit();
        return EXIT_SUCCESS;
    }
    logger.Info("Mounted bootstrap.pk3", 0);

    //main loop etc. is in Lua.
    std::string luaMainCode;
    if(!jar::fs::ReadFile("code/Bootstrap.lua", luaMainCode))
    {
        logger.Error("Could not read bootstrap.pk3/code/Bootstrap.lua:" + jar::fs::GetLastError());
        core.Deinit();
        return EXIT_SUCCESS;
    }
    logger.Info("Loaded code/Bootstrap.lua, now executing...\n", 0);

    if(!core.GetLua().ExecuteString(luaMainCode, "@code/Bootstrap.lua"))
    {
        logger.Error(core.GetLua().GetLastError());
    }

    core.Deinit();

    return 0;
}
