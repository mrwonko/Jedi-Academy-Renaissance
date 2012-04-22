#include "jar/input/InputDeviceController.hpp"
#include "jar/input/Event.hpp"
#include "jar/core/Logger.hpp"

namespace jar {

InputDeviceController::InputDeviceController(unsigned int index) :
    mIndex(index),
    mRumbleEnabled(true),
    mNumRumblers(0)
{
    //ctor
}

InputDeviceController::~InputDeviceController()
{
    //dtor
}

void InputDeviceController::SetRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceController::SetRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] = value;
}

void InputDeviceController::IncreaseRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceController::IncreaseRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] += value;
}

void InputDeviceController::DecreaseRumbleStrength(unsigned int index, float value)
{
    if(index > mNumRumblers) //validate index
    {
        Logger::GetDefaultLogger().Warning("Invalid Index for InputDeviceController::DecreaseRumbleStrength()!");
        return;
    }
    CheckRumbleStrengths(); //make sure mRumbleStrenghts has the right size
    mRumbleStrengths[index] -= value;
}

void InputDeviceController::CheckRumbleStrengths()
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

void InputDeviceController::JoyButtonPressed(unsigned int button)
{
    Event e;
    e.Type = Event::ControllerButtonPressed;
    e.ControllerButton.Button = button;
    e.ControllerButton.Controller = this;
    SendEvent(e);
}

void InputDeviceController::JoyButtonReleased(unsigned int button)
{
    Event e;
    e.Type = Event::ControllerButtonReleased;
    e.ControllerButton.Button = button;
    e.ControllerButton.Controller = this;
    SendEvent(e);
}

void InputDeviceController::JoyAxisMoved(unsigned int axis, float position)
{
    Event e;
    e.Type = Event::ControllerAxisMoved;
    e.ControllerAxis.Axis = axis;
    e.ControllerAxis.Position = position;
    e.ControllerAxis.Controller = this;
    SendEvent(e);
}

} // namespace jar
