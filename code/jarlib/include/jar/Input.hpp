#ifndef JAR_INPUT_HPP
#define JAR_INPUT_HPP

#include "jar/core/Singleton.hpp"
#include "jar/input/API.hpp"

namespace jar {

class InputImpl;
class InputDeviceManager;
class EventManager;
class RenderWindow;

class JARINPUTAPI Input : public Singleton<Input>
{
    public:
        Input();
        virtual ~Input();

        InputDeviceManager& GetInputDeviceManager();
        EventManager& GetEventManager();

    private:
        InputImpl* mImpl;

        friend class RenderWindow;
        void OnWindowCreated();
};

template<> JARINPUTAPI Input* Singleton<Input>::mSingleton;

} // namespace jar

#endif // JAR_INPUT_INPUT_HPP
