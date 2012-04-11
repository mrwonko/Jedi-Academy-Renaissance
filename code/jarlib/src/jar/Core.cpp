#include "jar/Core.hpp"
#include "jar/core/CoreImpl.hpp"

namespace jar {

Core* Core::mSingleton = NULL;

Core::Core() :
    mImpl(new CoreImpl)
{
    assert(mImpl);
    assert(!mSingleton);
    mSingleton = this;
}

Core::~Core()
{
    delete mImpl;
    assert(mSingleton);
    mSingleton = NULL;
}

const bool Core::Init(int argc, char** argv, const std::string& rootPath)
{
    return mImpl->Init(argc, argv, rootPath);
}

const bool Core::Deinit()
{
    return mImpl->Deinit();
}

const std::string& Core::GetRootPath()
{
    return mImpl->GetRootPath();
}

Lua& Core::GetLua()
{
    return mImpl->GetLua();
}

CoreImpl& Core::GetImpl()
{
    assert(mImpl);
    return *mImpl;
}

void Core::Update(const TimeType deltaT)
{
    mImpl->Update(deltaT);
}

} // namespace jar
