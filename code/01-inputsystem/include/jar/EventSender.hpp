#ifndef JAR_EVENTSENDER_HPP
#define JAR_EVENTSENDER_HPP

#include <set>

namespace jar
{

class Event;
class EventListener;
/** \brief Class for sending Events. Adds timestamps.
**/
class EventSender
{
    public:
        /** \brief Destructor **/
        virtual ~EventSender();

        /** \brief Registers a listener that will receive sent events
            \param listener The listener **/
        void RegisterListener(EventListener* listener);

        /** \brief Unregisters a listener

            \param listener the EventListener to unregister
            \return Whether such a listener existed
        **/
        const bool UnregisterListener(EventListener* listener);

    protected:
        /** \brief Constructor **/
        EventSender();

        /** \brief Sends an event to all registered listeners.
            \param event the Event to send **/
        void SendEvent(Event& event);

    private:
        std::set<EventListener*> mListeners; ///< the registered listeners
};

} // namespace jar

#endif // JAR_EVENTSENDER_HPP
