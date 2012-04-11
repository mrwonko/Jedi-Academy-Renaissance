#ifndef JAR_INPUT_EVENTMANAGER_HPP
#define JAR_INPUT_EVENTMANAGER_HPP

#include <jar/input/EventQueue.hpp>
#include <cassert>

namespace jar {

class JARINPUTAPI EventManager : public EventQueue
{
    public:
        EventManager();
        virtual ~EventManager();
        static EventManager& GetSingleton() { assert(mSingleton); return *mSingleton; }
        static const bool HasSingleton() { return mSingleton; }
    protected:
    private:
        static EventManager* mSingleton;
};

} // namespace jar

#endif // JAR_INPUT_EVENTMANAGER_HPP
