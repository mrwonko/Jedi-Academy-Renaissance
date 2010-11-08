#include "jar/core/FileSystem.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/CLArguments.hpp"
#include <physfs.h>

namespace jar
{

template<> FileSystem* Singleton<FileSystem>::mSingleton = NULL;

FileSystem::FileSystem()
{
    //ctor
}

FileSystem::~FileSystem()
{
    //dtor
}

const bool FileSystem::Init()
{
    if(PHYSFS_init(CLArguments::GetSingleton().GetExecutable().c_str()) == 0)
    {
        Logger::GetDefaultLogger().Error("FileSystem::Init(): "+std::string(PHYSFS_getLastError()));
        return false;
    }
    return true;
}

const bool FileSystem::Deinit()
{
    if(PHYSFS_deinit() == 0)
    {
        Logger::GetDefaultLogger().Error("FileSystem::Deinit(): "+std::string(PHYSFS_getLastError()));
        return false;
    }
    return true;
}


const bool FileSystem::Mount(const std::string& filename)
{
    if(PHYSFS_mount(filename.c_str(), NULL, 0) == 0)
    {
        Logger::GetDefaultLogger().Error("FileSystem::Mount(): "+std::string(PHYSFS_getLastError()));
        return false;
    }
    mFiles.insert(filename);
    return true;
}

const bool FileSystem::Unmount(const std::string& filename)
{
    if(PHYSFS_removeFromSearchPath(filename.c_str()) == 0)
    {
        Logger::GetDefaultLogger().Error("FileSystem::Unmount(): "+std::string(PHYSFS_getLastError()));
        return false;
    }
    std::set<std::string>::iterator find = mFiles.find(filename);
    if(find != mFiles.end())
    {
        mFiles.erase(find);
    }
    return true;
}

PHYSFS_File* FileSystem::OpenRead(const std::string& filename, const bool sendWarnings)
{
    /*
    PHYSFS_File* handle = PHYSFS_openRead(filename.c_str());
    if(handle == NULL && sendWarnings)
    {
        Logger::GetDefaultLogger().Warning(std::string(PHYSFS_getLastError()));
    }
    return handle;
    */
#error use FileSystem::File
}

} // namespace jar
