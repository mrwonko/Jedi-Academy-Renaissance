#include "jar/EventSender.hpp"
#include "jar/EventListener.hpp"
#include "jar/Event.hpp"
#include "jar/Time.hpp"
#include <cassert>

namespace jar {

EventSender::EventSender()
{
    //ctor
}

EventSender::~EventSender()
{
    //dtor
}

void EventSender::RegisterListener(EventListener* listener)
{
    assert(listener);
    mListeners.insert(listener);
}

const bool EventSender::UnregisterListener(EventListener* listener)
{
    for(std::set<EventListener*>::iterator it = mListeners.begin(); it != mListeners.end(); ++it)
    {
        if(*it == listener)
        {
            mListeners.erase(it);
            return true;
        }
    }
    return false;
}

void EventSender::SendEvent(Event& event)
{
    event.Timestamp = GetTime();
    for(std::set<EventListener*>::iterator it = mListeners.begin(); it != mListeners.end(); ++it)
    {
        (*it)->ReceiveEvent(event);
    }
}

} // namespace jar
