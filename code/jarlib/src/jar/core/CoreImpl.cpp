#include "jar/core/CoreImpl.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/CLArguments.hpp"
#include "jar/core/Lua.hpp"
#include "jar/core/FileSystem.hpp"
#include "jar/core/Component.hpp"
#include <jar/luabind/Core.hpp>
#include <luabind/luabind.hpp>

namespace jar {

CoreImpl::CoreImpl() :
    mCLArguments(0),
    mLua(0),
    mPhysFSInitialized(false)
{
    //ctor
}

CoreImpl::~CoreImpl()
{
    //dtor
}

const bool CoreImpl::Init(int argc, char** argv, const std::string& rootPath)
{
    mRootPath = rootPath;

    //init Command Line Arguments

    mCLArguments = new CLArguments(argc, argv);
    if(!mCLArguments)
    {
        Logger::GetDefaultLogger().Error("Could not allocate memory!");
        Deinit();
        return false;
    }

    //init Lua

    mLua = new Lua();
    if(!mLua)
    {
        Logger::GetDefaultLogger().Error("Could not allocate memory!");
        Deinit();
        return false;
    }
    if(!mLua->Init())
    {
        Logger::GetDefaultLogger().Error("Could not init Lua: "+mLua->GetLastError());
        Deinit();
        return false;
    }
    if(!mLua->OpenBaseLibrary() || !mLua->OpenMathLibrary() || !mLua->OpenStringLibrary() || !mLua->OpenTableLibrary() || !mLua->OpenPackageLibrary())
    {
        Logger::GetDefaultLogger().Error("Could not open Lua libraries: "+mLua->GetLastError());
        Deinit();
        return false;
    }
#ifdef _DEBUG
    if(!mLua->OpenDebugLibrary())
    {
        Logger::GetDefaultLogger().Error("Could not open Lua debug library: "+mLua->GetLastError());
        Deinit();
        return false;
    }
    luabind::globals(mLua->GetState())["DEBUG"] = true;
#endif
    Logger::GetDefaultLogger().Info("Initialized Lua", 1);

    jar::BindCore(mLua->GetState());
    Logger::GetDefaultLogger().Info("Exposed Core System to Lua", 1);


    //init physicsFS

    assert(argc > 0);
    if(!fs::Init(argv[0]))
    {
        Logger::GetDefaultLogger().Error("Cannot initialize filesystem: " + fs::GetLastError());
        Deinit();
        return false;
    }
    if(!fs::SetWriteDir("./"))
    {
        Logger::GetDefaultLogger().Error("Cannot set write directory: " + fs::GetLastError());
        Deinit();
        return false;
    }
    mPhysFSInitialized = true;
    Logger::GetDefaultLogger().Info("Initialized PhysicsFS", 1);

    Logger::GetDefaultLogger().Info("Core Systems successfully initialized!", 1);

    for(std::set<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
    {
        if(!(*it)->Init())
        {
            // init failed, so let's deinitialize everything
            Deinit();
            return false;
        }
    }
    return true;
}

const bool CoreImpl::Deinit()
{
    bool failed = false;
    for(std::set<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
    {
        if(!(*it)->Deinit())
        {
            failed = true;
        }
    }
    if(mPhysFSInitialized)
    {
        if(!fs::Deinit())
        {
            //now PhysicsFS is "probably badly screwed up".
            Logger::GetDefaultLogger().Error(std::string(fs::GetLastError()));
            failed = true;
        }
        mPhysFSInitialized = false;
    }
    if(mLua)
    {
        if(!mLua->Deinit())
        {
            Logger::GetDefaultLogger().Error("Could not deinitialize Lua: " + mLua->GetLastError());
            failed = true;
        }
        delete mLua;
    }
    if(mCLArguments)
    {
        delete mCLArguments;
    }
    Logger::GetDefaultLogger().Info("Core Systems successfully deinitialized!", 1);
    return !failed;
}

void CoreImpl::Update(const TimeType deltaT)
{
    for(std::set<Updatable*>::iterator it = mUpdatables.begin(); it != mUpdatables.end(); ++it)
    {
        (*it)->Update(deltaT);
    }
}

void CoreImpl::RegisterComponent(Component* c)
{
    mComponents.insert(c);
}

void CoreImpl::UnregisterComponent(Component* c)
{
    mComponents.erase(c);
}

void CoreImpl::RegisterUpdatable(Updatable* u)
{
    mUpdatables.insert(u);
}

void CoreImpl::UnregisterUpdatable(Updatable* u)
{
    mUpdatables.erase(u);
}

} // namespace jar
