#include "jar/input/EventSender.hpp"
#include "jar/input/EventListener.hpp"
#include "jar/input/Event.hpp"
#include "jar/core/Time.hpp"
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
    assert((void*)listener != (void*)this && "endless loop, tried to make EventSender send events to itself!");
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

void EventSender::SendEvent(const Event& event) const
{
    for(std::set<EventListener*>::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
    {
        (*it)->ReceiveEvent(event);
    }
}

} // namespace jar
