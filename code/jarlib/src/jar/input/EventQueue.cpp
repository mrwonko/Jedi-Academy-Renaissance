#include "jar/input/EventQueue.hpp"
#include "jar/input/Event.hpp"

namespace jar {

EventQueue::EventQueue()
{
    //ctor
}

EventQueue::~EventQueue()
{
    //dtor
}

void EventQueue::ReceiveEvent(const Event& event)
{
    mQueueMutex.Lock();
    mEventQueue.push(event);
    mQueueMutex.Unlock();
}

const bool EventQueue::GetEvent(Event& event)
{
    mQueueMutex.Lock();
    if(!mEventQueue.empty())
    {
        event = mEventQueue.front();
        mEventQueue.pop();
        mQueueMutex.Unlock();
        return true;
    }
    mQueueMutex.Unlock();
    return false;
}

} // namespace jar
