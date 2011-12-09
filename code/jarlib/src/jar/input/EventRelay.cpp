#include "jar/input/EventRelay.hpp"

namespace jar {

EventRelay::EventRelay()
{
    //ctor
}

EventRelay::~EventRelay()
{
    //dtor
}

void EventRelay::ReceiveEvent(const Event& e)
{
    SendEvent(e);
}

} // namespace jar
