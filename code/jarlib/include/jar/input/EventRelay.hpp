#ifndef JAR_INPUT_EVENTRELAY_HPP
#define JAR_INPUT_EVENTRELAY_HPP

#include "jar/input/EventListener.hpp"
#include "jar/input/EventSender.hpp"

namespace jar {

/** \brief An event listener that relays its received events to its listeners **/
class JARINPUTAPI EventRelay : public EventListener, public EventSender
{
    public:
        EventRelay();
        virtual ~EventRelay();

        virtual void ReceiveEvent(const Event& e);
    protected:
    private:
};

} // namespace jar

#endif // JAR_INPUT_EVENTRELAY_HPP
