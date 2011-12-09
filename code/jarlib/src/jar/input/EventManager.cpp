#include "jar/input/EventManager.hpp"

namespace jar {

template<> EventManager* Singleton<EventManager>::mSingleton = NULL;

EventManager::EventManager()
{
    //ctor
}

EventManager::~EventManager()
{
    //dtor
}

} // namespace jar
