#include "Config.h"
#include "Parser.h"

int main(int argc, char** argv)
{
    Config config;
    if(!config.Init(argc, argv))
    {
        return 0;
    }
    Parser parser(config);
    return 0;
}
