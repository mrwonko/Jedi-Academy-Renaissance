#include "Filesystem.h"

Filesystem::Filesystem(const std::list<std::string>& includeDirs) :
    mIncludeDirs(includeDirs)
{
    //ctor
}

Filesystem::~Filesystem()
{
    //dtor
}

std::string Filesystem::FindFile(const std::string& filename)
{
    //try relative/absolute
    std::ifstream file(filename.c_str());
    if(file.good())
    {
        return filename;
    }
    //now try all the include directories
    for(std::list<std::string>::const_iterator dirIt = mIncludeDirs.begin(); dirIt != mIncludeDirs.end(); ++dirIt)
    {
        std::string curFilename = *dirIt + filename;
        file.open(curFilename.c_str());
        if(file.good())
        {
            return curFilename;
        }
    }
    //no "good" filestream was found
    return "";
}
