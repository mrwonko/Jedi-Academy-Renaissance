#include "jar/InputDeviceJoystick.hpp"
#include "jar/Event.hpp"

namespace jar {

InputDeviceJoystick::InputDeviceJoystick() :
    Index(0)
{
    //ctor
}

InputDeviceJoystick::~InputDeviceJoystick()
{
    //dtor
}

void InputDeviceJoystick::JoyButtonPressed(unsigned int button)
{
    Event e;
    e.Type = Event::JoyButtonPressed;
    e.JoyButton.Button = button;
    SendEvent(e);
}

void InputDeviceJoystick::JoyButtonReleased(unsigned int button)
{
    Event e;
    e.Type = Event::JoyButtonReleased;
    e.JoyButton.Button = button;
    SendEvent(e);
}

void InputDeviceJoystick::JoyAxisMoved(unsigned int axis, float position)
{
    Event e;
    e.Type = Event::JoyAxisMoved;
    e.JoyAxis.Axis = axis;
    e.JoyAxis.Position = position;
    SendEvent(e);
}

} // namespace jar
