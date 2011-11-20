#ifndef PARSER_H
#define PARSER_H

#include "Filesystem.h"

class Config;

class Parser
{
    public:
        /** \param config The configuration to use - stored as a reference so keep alive! **/
        Parser(const Config& config);
        virtual ~Parser();

    private:
        const Config& mConfig;
        Filesystem mFilesystem;
};

#endif // PARSER_H
