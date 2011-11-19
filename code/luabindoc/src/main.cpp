#include "Config.h"

int main(int argc, char** argv)
{
    Config config;
    if(!config.Init(argc, argv))
    {
        return 0;
    }
    return 0;
}
