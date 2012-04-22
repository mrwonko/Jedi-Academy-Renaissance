#include "jar/input/EventManager.hpp"

namespace jar {

EventManager* EventManager::mSingleton = NULL;

EventManager::EventManager()
{
    //ctor
    assert(!mSingleton);
    mSingleton = this;
}

EventManager::~EventManager()
{
    //dtor
    assert(mSingleton);
    mSingleton = NULL;
}

} // namespace jar
