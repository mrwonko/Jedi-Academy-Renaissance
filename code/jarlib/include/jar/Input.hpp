#ifndef JAR_INPUT_HPP
#define JAR_INPUT_HPP

#include <jar/input/API.hpp>
#include <cassert>

namespace jar {

class InputImpl;
class InputDeviceManager;
class EventManager;
class RenderWindow;

class JARINPUTAPI Input
{
    public:
        Input();
        virtual ~Input();

        InputDeviceManager& GetInputDeviceManager();
        EventManager& GetEventManager();

        static Input& GetSingleton() { assert(mSingleton); return *mSingleton; }

    private:
        InputImpl* mImpl;

        friend class RenderWindow;
        void OnWindowCreated();

        static Input* mSingleton;
};

} // namespace jar

#endif // JAR_INPUT_INPUT_HPP
