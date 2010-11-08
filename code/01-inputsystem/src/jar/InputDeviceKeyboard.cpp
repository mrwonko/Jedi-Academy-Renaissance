#include "jar/InputDeviceKeyboard.hpp"
#include "jar/Event.hpp"

namespace jar
{

InputDeviceKeyboard::InputDeviceKeyboard()
{
    //ctor
    mName = "Keyboard";
}

InputDeviceKeyboard::~InputDeviceKeyboard()
{
    //dtor
}

void InputDeviceKeyboard::KeyPressed(Key::Code code)
{
    Event e;
    e.Type = Event::KeyPressed;
    e.Key.Code = code;
    SendEvent(e);
}

void InputDeviceKeyboard::KeyReleased(Key::Code code)
{
    Event e;
    e.Type = Event::KeyReleased;
    e.Key.Code = code;
    SendEvent(e);
}

} // namespace jar
