#ifndef JAR_INPUT_EVENTMANAGER_HPP
#define JAR_INPUT_EVENTMANAGER_HPP

#include "jar/input/EventQueue.hpp"
#include "jar/core/Singleton.hpp"

namespace jar {

class JARINPUTAPI EventManager : public EventQueue, public Singleton<EventManager>
{
    public:
        EventManager();
        virtual ~EventManager();
    protected:
    private:
};

template<> JARINPUTAPI EventManager* Singleton<EventManager>::mSingleton;

} // namespace jar

#endif // JAR_INPUT_EVENTMANAGER_HPP
