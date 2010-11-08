#ifndef JAR_EVENTQUEUE_HPP
#define JAR_EVENTQUEUE_HPP

#include <SFML/System/Mutex.hpp>
#include <queue>
#include "jar/EventListener.hpp"

namespace jar
{

/** \brief A thread-safe Queue of Events **/
class EventQueue : public EventListener
{
    public:
        EventQueue();
        virtual ~EventQueue();

        /** \brief Retreives an event, if there is any.
         *
         * \param out_event The event will be put here, if there is one.
         * \return Whether there was any event left. If yes, the first is now in event.
         *
         */
        const bool GetEvent(Event& out_event);
        void ReceiveEvent(const Event& event);
    private:
        std::queue<Event> mEventQueue;
        sf::Mutex mQueueMutex;
};

} // namespace jar

#endif // JAR_EVENTQUEUE_HPP
