#include "jar/input/InputDeviceJoystick.hpp"
#include "jar/input/Event.hpp"
#include "jar/core/Logger.hpp"

namespace jar {

InputDeviceJoystick::InputDeviceJoystick(unsigned int index) :
    mIndex(index),
    mRumbleEnabled(true),
    mNumRumblers(0)
{
    //ctor
}

InputDeviceJoystick::~InputDeviceJoystick()
{
    //dtor
}

void InputDeviceJoystick::SetRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceJoystick::SetRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] = value;
}

void InputDeviceJoystick::IncreaseRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceJoystick::IncreaseRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] += value;
}

void InputDeviceJoystick::DecreaseRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceJoystick::DecreaseRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] -= value;
}

void InputDeviceJoystick::CheckRumbleStrengths()
{
    while(mRumbleStrengths.size() < mNumRumblers)
    {
        mRumbleStrengths.push_back(0.0f);
    }
    while(mRumbleStrengths.size() > mNumRumblers)
    {
        mRumbleStrengths.pop_back();
    }
}

void InputDeviceJoystick::JoyButtonPressed(unsigned int button)
{
    Event e;
    e.Type = Event::JoyButtonPressed;
    e.JoyButton.Button = button;
    e.JoyButton.Joystick = this;
    SendEvent(e);
}

void InputDeviceJoystick::JoyButtonReleased(unsigned int button)
{
    Event e;
    e.Type = Event::JoyButtonReleased;
    e.JoyButton.Button = button;
    e.JoyButton.Joystick = this;
    SendEvent(e);
}

void InputDeviceJoystick::JoyAxisMoved(unsigned int axis, float position)
{
    Event e;
    e.Type = Event::JoyAxisMoved;
    e.JoyAxis.Axis = axis;
    e.JoyAxis.Position = position;
    e.JoyButton.Joystick = this;
    SendEvent(e);
}

} // namespace jar
