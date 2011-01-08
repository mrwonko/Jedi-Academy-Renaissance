#include <jar/Core.hpp>
#include <jar/Core/CoutLogger.hpp>
#include <jar/Core/Lua.hpp>

#include <luabind/luabind.hpp>
#include <luabind/iterator_policy.hpp>

#include <string>
#include <vector>

struct ReadOnly
{
    int number;
};

int main(int argc, char** argv)
{
    //create Logger
    jar::CoutLogger logger;
    logger.SetLoggingLevel(5);
    logger.Info("Initialized Logger", 0);

    //initialize core & other components
    jar::Core core;

    if(!core.Init(argc, argv, "./"))
    {
        logger.Error("Could not initialize!");
        return 0;
    }

    luabind::module(core.GetLua().GetState())
    [
        luabind::class_<ReadOnly>("ReadOnly")
            .def(luabind::constructor<>())
            .def_readonly("number", &ReadOnly::number)
    ];

    std::string code = "--lua test stuff\n\
print(\"===Lua Test code goes here===\") \n\
 \n\
";

    if(!core.GetLua().ExecuteString(code, "Testcode"))
    {
        logger.Error(core.GetLua().GetLastError());
    }

    return 0;
}
