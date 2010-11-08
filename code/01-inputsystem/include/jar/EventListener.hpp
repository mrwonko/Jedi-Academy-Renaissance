#ifndef JAR_EVENTLISTENER_H
#define JAR_EVENTLISTENER_H

namespace jar
{

class Event;

/** \brief Event Listeners are objects receiving events.
 *
 * Inherit EventListener and implement ReceiveEvent().
 *
 */
class EventListener
{
    public:
        //virtual destructor since people inherit from this
        /** \brief destructor **/
        virtual ~EventListener() {}

        /** \brief Called when an Event is received.
            \param event The Event
        **/
        virtual void ReceiveEvent(const Event& event) = 0;
    protected:
        //since you need to inherit this, the constructor's protected
        /** \brief constructor **/
        EventListener() {}
};

} // namespace jar

#endif // JAR_EVENTLISTENER_H
