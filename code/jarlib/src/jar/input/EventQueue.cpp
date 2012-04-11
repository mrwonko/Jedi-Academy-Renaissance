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
    mQueueMutex.lock();
    mEventQueue.push(event);
    mQueueMutex.unlock();
}

const bool EventQueue::GetEvent(Event& event)
{
    mQueueMutex.lock();
    if(!mEventQueue.empty())
    {
        event = mEventQueue.front();
        mEventQueue.pop();
        mQueueMutex.unlock();
        return true;
    }
    mQueueMutex.unlock();
    return false;
}

} // namespace jar
