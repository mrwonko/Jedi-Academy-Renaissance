/*
===========================================================================
Copyright (C) 2010 Willi Schinmeyer

This file is part of the Jedi Academy: Renaissance source code.

Jedi Academy: Renaissance source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Jedi Academy: Renaissance source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jedi Academy: Renaissance source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "jar/input/Windows/WinJoystickDirectInput.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/Input.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"

namespace jar {
namespace Windows {

WinJoystickDirectInput::WinJoystickDirectInput() :
    mDevice(NULL)
{
    //ctor
    for(int i = 0; i <32; ++i)
    {
        mRangeInfo[i].max = 0;
        mRangeInfo[i].min = 0;
    }
}

WinJoystickDirectInput::~WinJoystickDirectInput()
{
    //dtor
}

struct DIControlObjectSharedInfo
{
    DIControlObjectSharedInfo(const std::string& name) : Name(name), Failed(false) {}
    const std::string& Name;
    LPDIRECTINPUTDEVICE8 Device;
    WinJoystickDirectInput::AxisRangeInfo* Ranges;
    bool Failed;
};

static __stdcall BOOL ForEachAbsAxis(LPCDIDEVICEOBJECTINSTANCE obj, LPVOID voidptr)
{
    DIControlObjectSharedInfo* info = static_cast<DIControlObjectSharedInfo*>(voidptr);

    int axis = -1;
    if(obj->guidType == GUID_XAxis) axis = WinJoystickDirectInput::AxisX;
    if(obj->guidType == GUID_YAxis) axis = WinJoystickDirectInput::AxisY;
    if(obj->guidType == GUID_ZAxis) axis = WinJoystickDirectInput::AxisZ;
    if(obj->guidType == GUID_RxAxis) axis = WinJoystickDirectInput::AxisRX;
    if(obj->guidType == GUID_RyAxis) axis = WinJoystickDirectInput::AxisRY;
    if(obj->guidType == GUID_RzAxis) axis = WinJoystickDirectInput::AxisRZ;
    if(obj->guidType == GUID_Slider) axis = WinJoystickDirectInput::AxisSlider0;

    //ignore unknown axes
    if(axis == -1) return DIENUM_CONTINUE;

    //axis range - thanks http://efreedom.com/Question/1-1053702/DirectInput-Analogue-Joystick-Range
    DIPROPRANGE range;
    range.diph.dwSize = sizeof(DIPROPRANGE);
    range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    range.diph.dwHow = DIPH_BYID;
    range.diph.dwObj = obj->dwType;
    if(info->Device->GetProperty(DIPROP_RANGE, &range.diph) != DI_OK)
    {
        Logger::GetDefaultLogger().Error("Error while getting object info for Input Device \""+info->Name+"\"!");
        info->Failed = true;
        return DIENUM_STOP;
    }

    Logger::GetDefaultLogger().Info(info->Name + " Axis " + Helpers::IntToString(axis) + ": " + Helpers::IntToString(range.lMin) + " - " + Helpers::IntToString(range.lMax), 5);

    info->Ranges[axis].min = range.lMin;
    info->Ranges[axis].max = range.lMax;
    //since most axes don't have a range defined I'm going to assume similar axes have similar ranges. Problem? Probably. Or is there?
    if(axis == WinJoystickDirectInput::AxisSlider0)
    {
        info->Ranges[WinJoystickDirectInput::AxisSlider1].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisSlider1].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisVSlider0].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisVSlider0].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisVSlider1].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisVSlider1].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisASlider0].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisASlider0].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisASlider1].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisASlider1].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisFSlider0].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisFSlider0].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisFSlider1].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisFSlider1].max = range.lMax;
    }
    else if(axis == WinJoystickDirectInput::AxisX)
    {
        info->Ranges[WinJoystickDirectInput::AxisVX].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisVX].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisAX].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisAX].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisFX].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisFX].max = range.lMax;
    }
    else if(axis == WinJoystickDirectInput::AxisY)
    {
        info->Ranges[WinJoystickDirectInput::AxisVY].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisVY].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisAY].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisAY].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisFY].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisFY].max = range.lMax;
    }
    else if(axis == WinJoystickDirectInput::AxisZ)
    {
        info->Ranges[WinJoystickDirectInput::AxisVZ].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisVZ].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisAZ].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisAZ].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisFZ].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisFZ].max = range.lMax;
    }
    else if(axis == WinJoystickDirectInput::AxisRX)
    {
        info->Ranges[WinJoystickDirectInput::AxisVRX].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisVRX].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisARX].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisARX].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisFRX].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisFRX].max = range.lMax;
    }
    else if(axis == WinJoystickDirectInput::AxisRY)
    {
        info->Ranges[WinJoystickDirectInput::AxisVRY].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisVRY].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisARY].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisARY].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisFRY].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisFRY].max = range.lMax;
    }
    else if(axis == WinJoystickDirectInput::AxisRZ)
    {
        info->Ranges[WinJoystickDirectInput::AxisVRZ].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisVRZ].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisARZ].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisARZ].max = range.lMax;
        info->Ranges[WinJoystickDirectInput::AxisFRZ].min = range.lMin;
        info->Ranges[WinJoystickDirectInput::AxisFRZ].max = range.lMax;
    }
    return DIENUM_CONTINUE;
}

const bool WinJoystickDirectInput::Init(LPDIRECTINPUT8 directInput, LPCDIDEVICEINSTANCE deviceInfo)
{
    mName = deviceInfo->tszProductName;
    mGUID = deviceInfo->guidProduct;

    HRESULT result;

    //create device

    if((result = directInput->CreateDevice(mGUID, &mDevice, NULL)) != DI_OK)
    {
        Logger::GetDefaultLogger().Error("Could not create Input Device \"" + mName + "\"!");
        Deinit();
        return false;
    }

    //  get device info

    //check each axis for range

    DIControlObjectSharedInfo info(mName);
    info.Device = mDevice;
    info.Ranges = mRangeInfo;

    if((result = mDevice->EnumObjects(ForEachAbsAxis, static_cast<LPVOID>(&info), DIDFT_ABSAXIS)) != DI_OK)
    {
        Logger::GetDefaultLogger().Error("Could not get info about axes of Input Device \"" + mName + "\"!");
        Deinit();
        return false;
    }
    if(info.Failed)
    {
        Deinit();
        return false;
    }

    //get general information
    DIDEVCAPS devcaps;
    devcaps.dwSize = sizeof(devcaps);
    if((result = mDevice->GetCapabilities(&devcaps)) != DI_OK)
    {
        std::string err = "Unknown error!";
        if(result == DIERR_INVALIDPARAM) err = "Invalid parameter!";
        else if(result == DIERR_NOTINITIALIZED) err = "Not initialized!";
        else if(result == E_POINTER) err = "Pointer error!";
        Logger::GetDefaultLogger().Error("Could not get info about Input Device \"" + mName + "\":" + err);
        Deinit();
        return false;
    }
    //I specifically only query for attached devices. The device shouldn't have been disconnected since calling Init().
    assert(devcaps.dwFlags & DIDC_ATTACHED && "Input Device not attached anymore? It ought to be!");

    mNumRumblers = (devcaps.dwFlags & DIDC_FORCEFEEDBACK) ? 1 : 0;
    mNumAxes = devcaps.dwAxes;
    mNumButtons = devcaps.dwButtons;
    mNumPOVs = devcaps.dwPOVs;

    if(mNumPOVs > 1)
    {
        Logger::GetDefaultLogger().Warning("Input Device \"" + mName + "\" has more than 1 DPad. Only 1 is supported, the others will not work.");
    }

    //  setup for input polling

    //I may need to call mDevice->SetCooperativeLevel() first, but let's try not using it since it requires a window.

    //set the data to be very elaborate :-) maaany axes and buttons
    if((result = mDevice->SetDataFormat(&c_dfDIJoystick2)) != DI_OK)
    {
        Logger::GetDefaultLogger().Error("Could not set Data Format for Input Device \"" + mName + "\"!");
        Deinit();
        return false;
    }

    if((result = mDevice->Acquire()) != DI_OK)
    {
        Logger::GetDefaultLogger().Error("Could not acquire Input Device \"" + mName + "\"!");
        Deinit();
        return false;
    }

    if((result = mDevice->GetDeviceState(sizeof(mLastState), &mLastState)) != DI_OK)
    {
        Logger::GetDefaultLogger().Error("Could not do test query on Input Device \"" + mName + "\"!");
        Deinit();
        return false;
    }

    return true;
}

const bool WinJoystickDirectInput::Deinit()
{
    if(mDevice)
    {
        mDevice->Unacquire(); //does nothing if not acquired
        mDevice->Release();
        mDevice = NULL; //not sure if this is necessary, but it won't hurt.
    }
    return true;
}

void WinJoystickDirectInput::Update(TimeType deltaT)
{
    HRESULT result;

    //  input polling

    DIJOYSTATE2 state;
    if((result = mDevice->GetDeviceState(sizeof(state), &state)) != DI_OK)
    {
        Logger::GetDefaultLogger().Warning("Could not update Input Device \"" + mName + "\", ignoring it.");
        Input::GetSingleton().GetInputDeviceManager().RemoveInputDevice(this);
        Deinit(); //may as well unacquire & release it now, if possible.
        return;
    }

    //check buttons
    for(unsigned int i = 0; i < 128; ++i)
    {
        //lower 16 buttons are reserved for POV Buttons, see class notes.
        CheckButtonChange(i+16, mLastState.rgbButtons[i], state.rgbButtons[i]);
    }

    //check POVs
    for(unsigned int i = 0; i < 4; ++i)
    {
        CheckPOVChange(i, mLastState.rgdwPOV[i], state.rgdwPOV[i]);
    }

    //check axes
    CheckAxisChange(AxisX, mLastState.lX, state.lX);
    CheckAxisChange(AxisY, mLastState.lY, state.lY);
    CheckAxisChange(AxisRX, mLastState.lRx, state.lRx);
    CheckAxisChange(AxisRY, mLastState.lRy, state.lRy);
    CheckAxisChange(AxisZ, mLastState.lZ, state.lZ); //switched order so it's like in XInput
    CheckAxisChange(AxisRZ, mLastState.lRz, state.lRz);
    CheckAxisChange(AxisSlider0, mLastState.rglSlider[0], state.rglSlider[0]);
    CheckAxisChange(AxisSlider1, mLastState.rglSlider[1], state.rglSlider[1]);
    CheckAxisChange(AxisVX, mLastState.lVX, state.lVX);
    CheckAxisChange(AxisVY, mLastState.lVY, state.lVY);
    CheckAxisChange(AxisVZ, mLastState.lVZ, state.lVZ);
    CheckAxisChange(AxisVRX, mLastState.lVRx, state.lVRx);
    CheckAxisChange(AxisVRY, mLastState.lVRy, state.lVRy);
    CheckAxisChange(AxisVRZ, mLastState.lVRz, state.lVRz);
    CheckAxisChange(AxisVSlider0, mLastState.rglVSlider[0], state.rglVSlider[0]);
    CheckAxisChange(AxisVSlider1, mLastState.rglVSlider[1], state.rglVSlider[1]);
    CheckAxisChange(AxisAX, mLastState.lAX, state.lAX);
    CheckAxisChange(AxisAY, mLastState.lAY, state.lAY);
    CheckAxisChange(AxisAZ, mLastState.lAZ, state.lAZ);
    CheckAxisChange(AxisARX, mLastState.lARx, state.lARx);
    CheckAxisChange(AxisARY, mLastState.lARy, state.lARy);
    CheckAxisChange(AxisARZ, mLastState.lARz, state.lARz);
    CheckAxisChange(AxisASlider0, mLastState.rglASlider[0], state.rglASlider[0]);
    CheckAxisChange(AxisASlider1, mLastState.rglASlider[1], state.rglASlider[1]);
    CheckAxisChange(AxisFX, mLastState.lFX, state.lFX);
    CheckAxisChange(AxisFY, mLastState.lFY, state.lFY);
    CheckAxisChange(AxisFZ, mLastState.lFZ, state.lFZ);
    CheckAxisChange(AxisFRX, mLastState.lFRx, state.lFRx);
    CheckAxisChange(AxisFRY, mLastState.lFRy, state.lFRy);
    CheckAxisChange(AxisFRZ, mLastState.lFRz, state.lFRz);
    CheckAxisChange(AxisFSlider0, mLastState.rglFSlider[0], state.rglFSlider[0]);
    CheckAxisChange(AxisSlider1, mLastState.rglFSlider[1], state.rglFSlider[1]);

    mLastState = state;

    //  force feedback

    if(mNumRumblers > 0)
    {
        assert(mNumRumblers == 1);
        //TODO: DirectInput rumbling
    }
}

void WinJoystickDirectInput::CheckButtonChange(unsigned int buttonIndex, BYTE oldState, BYTE newState)
{
    //check whether a button is pressed: state.rgbButtons[i] & 0x80 (?)
    if(oldState != newState)
    {
        if(newState & 0x80)
        {
            JoyButtonPressed(buttonIndex);
        }
        else
        {
            JoyButtonReleased(buttonIndex);
        }
    }
}

void WinJoystickDirectInput::CheckAxisChange(Axis axisIndex, LONG oldState, LONG newState)
{
    assert(axisIndex < AxisLAST);
    if(oldState != newState)
    {
        if (mRangeInfo[axisIndex].min == mRangeInfo[axisIndex].max)
        {
            Logger::GetDefaultLogger().Warning("Input Device \"" + mName + "\" has an invalid axis "+ Helpers::IntToString(axisIndex)+" range, still reports axis movements, ignoring the device.");
            Input::GetSingleton().GetInputDeviceManager().RemoveInputDevice(this);
            Deinit(); //may as well unacquire & release it now, if possible.
            return;
        }
        //mapping max to 1 and min to -1 (linear):
        JoyAxisMoved(axisIndex,

                     (( ((float)newState) - mRangeInfo[axisIndex].min ) * 2
                                                 /
                     ( (float)(mRangeInfo[axisIndex].max) - mRangeInfo[axisIndex].min )           -1)

                     );
    }
}

void WinJoystickDirectInput::CheckPOVChange(unsigned int povIndex, DWORD oldState, DWORD newState)
{
    if(oldState != newState)
    {
        if(POVIsUp(newState) != POVIsUp(oldState)) POVIsUp(newState) ? JoyButtonPressed(povIndex*4) : JoyButtonReleased(povIndex*4);
        if(POVIsDown(newState) != POVIsDown(oldState)) POVIsDown(newState) ? JoyButtonPressed(povIndex*4+1) : JoyButtonReleased(povIndex*4+1);
        if(POVIsLeft(newState) != POVIsLeft(oldState)) POVIsLeft(newState) ? JoyButtonPressed(povIndex*4+2) : JoyButtonReleased(povIndex*4+2);
        if(POVIsRight(newState) != POVIsRight(oldState)) POVIsRight(newState) ? JoyButtonPressed(povIndex*4+3) : JoyButtonReleased(povIndex*4+3);
    }
}

} // namespace Windows
} // namespace jar
