#include "jar/input/InputDeviceMouse.hpp"
#include "jar/input/Event.hpp"

namespace jar
{

InputDeviceMouse::InputDeviceMouse()
{
    //ctor
    mName = "Mouse";
}

InputDeviceMouse::~InputDeviceMouse()
{
    //dtor
}

void InputDeviceMouse::MouseButtonPressed(unsigned int button)
{
    Event e;
    e.Type = Event::MouseButtonPressed;
    e.MouseButton.Button = button;
    SendEvent(e);
}

void InputDeviceMouse::MouseButtonReleased(unsigned int button)
{
    Event e;
    e.Type = Event::MouseButtonReleased;
    e.MouseButton.Button = button;
    SendEvent(e);
}

void InputDeviceMouse::MouseMoved(int x, int y)
{
    Event e;
    e.Type = Event::MouseMoved;
    e.MouseMove.X = x;
    e.MouseMove.Y = y;
    SendEvent(e);
}

} // namespace jar
