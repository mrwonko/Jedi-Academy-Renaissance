#ifndef JAR_INPUT_INPUTIMPL_HPP
#define JAR_INPUT_INPUTIMPL_HPP

#include "jar/core/Component.hpp"

namespace jar {

class InputDeviceManager;

#ifdef _WIN32
namespace Windows
{
    class WinKeyboard;
}
#endif

class InputImpl : public Component
{
    public:
        InputImpl();
        virtual ~InputImpl();

        virtual const bool Init();
        virtual const bool Deinit();

        virtual void Update(TimeType deltaT);

    private:
        InputDeviceManager* mInputDeviceManager;
#ifdef WIN32
        Windows::WinKeyboard* mKeyboard;
#endif
};

} // namespace jar

#endif // JAR_INPUT_INPUTIMPL_HPP
