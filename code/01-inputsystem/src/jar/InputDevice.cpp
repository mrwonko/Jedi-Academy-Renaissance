#include "jar/InputDevice.hpp"

namespace jar
{

InputDevice::InputDevice() :
    mName("Unnamed!")
{
    //ctor
}

InputDevice::~InputDevice()
{
    //dtor
}

const bool InputDevice::Init()
{
    return true;
}

void InputDevice::Update(TimeType deltaT)
{
    //nothing done here, usually
}

} // namespace jar
