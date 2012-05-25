#include "jar/input/Windows/WinControllerDirectInput.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/Input.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"

#include <sstream>

namespace jar {
namespace Windows {

WinControllerDirectInput::WinControllerDirectInput(unsigned int index) :
    InputDeviceController(index),
    mDevice(NULL),
    mNumAxes(0),
    mNumButtons(0),
    mNumPOVs(0),
    mIsExclusive(false)
{
    //ctor
    for(int i = 0; i <32; ++i)
    {
        mRangeInfo[i].max = 0;
        mRangeInfo[i].min = 0;
    }
}

WinControllerDirectInput::~WinControllerDirectInput()
{
    //dtor
}

namespace
{
    struct DIControlObjectSharedInfo
    {
        DIControlObjectSharedInfo(const std::string& name, std::vector<DWORD>& ffAxes) : Name(name), Ranges(NULL), Failed(false), FFAxes(ffAxes) {}
        const std::string& Name;
        LPDIRECTINPUTDEVICE8 Device;
        WinControllerDirectInput::AxisRangeInfo* Ranges;
        bool Failed;
        std::vector<DWORD>& FFAxes;
    };

    static BOOL __stdcall ForEachAbsAxis(LPCDIDEVICEOBJECTINSTANCE obj, LPVOID voidptr)
    {
        DIControlObjectSharedInfo* info = static_cast<DIControlObjectSharedInfo*>(voidptr);

        int axis = -1;
        if(obj->guidType == GUID_XAxis) axis = WinControllerDirectInput::AxisX;
        if(obj->guidType == GUID_YAxis) axis = WinControllerDirectInput::AxisY;
        if(obj->guidType == GUID_ZAxis) axis = WinControllerDirectInput::AxisZ;
        if(obj->guidType == GUID_RxAxis) axis = WinControllerDirectInput::AxisRX;
        if(obj->guidType == GUID_RyAxis) axis = WinControllerDirectInput::AxisRY;
        if(obj->guidType == GUID_RzAxis) axis = WinControllerDirectInput::AxisRZ;
        if(obj->guidType == GUID_Slider) axis = WinControllerDirectInput::AxisSlider0;

        //ignore unknown axes
        if(axis == -1) return DIENUM_CONTINUE;

        //force feedback
        if(obj->dwFlags & DIDOI_FFACTUATOR)
        {
            switch(axis)
            {
                case WinControllerDirectInput::AxisX:
                {
                    info->FFAxes.push_back(DIJOFS_X);
                    break;
                }
                case WinControllerDirectInput::AxisY:
                {
                    info->FFAxes.push_back(DIJOFS_Y);
                    break;
                }
                case WinControllerDirectInput::AxisZ:
                {
                    info->FFAxes.push_back(DIJOFS_Z);
                    break;
                }
                case WinControllerDirectInput::AxisRX:
                {
                    info->FFAxes.push_back(DIJOFS_RX);
                    break;
                }
                case WinControllerDirectInput::AxisRY:
                {
                    info->FFAxes.push_back(DIJOFS_RY);
                    break;
                }
                case WinControllerDirectInput::AxisRZ:
                {
                    info->FFAxes.push_back(DIJOFS_RZ);
                    break;
                }
                case WinControllerDirectInput::AxisSlider0:
                {
                    info->FFAxes.push_back(DIJOFS_SLIDER(0));
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

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
        if(axis == WinControllerDirectInput::AxisSlider0)
        {
            info->Ranges[WinControllerDirectInput::AxisSlider1].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisSlider1].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisVSlider0].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisVSlider0].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisVSlider1].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisVSlider1].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisASlider0].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisASlider0].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisASlider1].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisASlider1].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisFSlider0].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisFSlider0].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisFSlider1].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisFSlider1].max = range.lMax;
        }
        else if(axis == WinControllerDirectInput::AxisX)
        {
            info->Ranges[WinControllerDirectInput::AxisVX].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisVX].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisAX].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisAX].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisFX].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisFX].max = range.lMax;
        }
        else if(axis == WinControllerDirectInput::AxisY)
        {
            info->Ranges[WinControllerDirectInput::AxisVY].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisVY].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisAY].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisAY].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisFY].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisFY].max = range.lMax;
        }
        else if(axis == WinControllerDirectInput::AxisZ)
        {
            info->Ranges[WinControllerDirectInput::AxisVZ].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisVZ].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisAZ].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisAZ].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisFZ].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisFZ].max = range.lMax;
        }
        else if(axis == WinControllerDirectInput::AxisRX)
        {
            info->Ranges[WinControllerDirectInput::AxisVRX].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisVRX].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisARX].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisARX].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisFRX].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisFRX].max = range.lMax;
        }
        else if(axis == WinControllerDirectInput::AxisRY)
        {
            info->Ranges[WinControllerDirectInput::AxisVRY].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisVRY].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisARY].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisARY].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisFRY].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisFRY].max = range.lMax;
        }
        else if(axis == WinControllerDirectInput::AxisRZ)
        {
            info->Ranges[WinControllerDirectInput::AxisVRZ].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisVRZ].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisARZ].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisARZ].max = range.lMax;
            info->Ranges[WinControllerDirectInput::AxisFRZ].min = range.lMin;
            info->Ranges[WinControllerDirectInput::AxisFRZ].max = range.lMax;
        }
        return DIENUM_CONTINUE;
    }

    struct DIEffectsSharedInfo
    {
        DIEffectsSharedInfo(const std::string& name) :
            Name(name),
            Capable(false)
        {}

        const std::string& Name;
        bool Capable;
        GUID Guid;
    };

    //according to MSDN it'S DIEffectInfo, but according to the mingw-w64 headers it'S DIEFFECTINFO. Since I want my code to compile, I use the latter.
    static BOOL __stdcall DIEnumEffectsCallback(const DIEFFECTINFO* info, void* pvRef)
    {
        DIEffectsSharedInfo* sharedInfo = static_cast<DIEffectsSharedInfo*>(pvRef);

        if(DIDFT_GETTYPE(info->dwEffType) == DIEFT_CONSTANTFORCE)
        {
            Logger::GetDefaultLogger().Info("Input Device \"" + sharedInfo->Name + "\" has a constant force rumble effect called \"" + info->tszName + "\", enabling rumbling.", 3);
            sharedInfo->Capable = true;
            sharedInfo->Guid = info->guid;
            return DIENUM_STOP;
        }
        return DIENUM_CONTINUE;
    }

}

const bool WinControllerDirectInput::Init(LPDIRECTINPUT8 directInput, LPCDIDEVICEINSTANCE deviceInfo)
{
    mName = deviceInfo->tszProductName;
    mGUID = deviceInfo->guidInstance;

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

    DIControlObjectSharedInfo coinfo(mName, mFFAxes);
    coinfo.Device = mDevice;
    coinfo.Ranges = mRangeInfo;

    if((result = mDevice->EnumObjects(ForEachAbsAxis, static_cast<LPVOID>(&coinfo), DIDFT_ABSAXIS)) != DI_OK)
    {
        Logger::GetDefaultLogger().Error("Could not get info about axes of Input Device \"" + mName + "\"!");
        Deinit();
        return false;
    }
    if(coinfo.Failed)
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

    DIEffectsSharedInfo einfo(mName);

    if((result = mDevice->EnumEffects(DIEnumEffectsCallback, static_cast<LPVOID>(&einfo), DIEFT_CONSTANTFORCE)) != DI_OK)
    {
        Logger::GetDefaultLogger().Warning("Could not enumerate Force Feedback effects of Input Device \"" + mName + "\"!");
    }

    if(einfo.Capable)
    {
        mNumRumblers = mFFAxes.size();
        for(unsigned int i = 0; i < mNumRumblers; ++i)
        {
            mLastRumbleStrengths.push_back(0.0f);
        }
        CheckRumbleStrengths();
        mRumbleEffectGUID = einfo.Guid;
    }

    if((result = mDevice->GetDeviceState(sizeof(mLastState), &mLastState)) != DI_OK)
    {
        Logger::GetDefaultLogger().Error("Could not do test query on Input Device \"" + mName + "\"!");
        Deinit();
        return false;
    }


    return true;
}

const bool WinControllerDirectInput::Deinit()
{
    if(!mRumbleEffects.empty())
    {
        for(unsigned int i = 0; i < mRumbleEffects.size(); ++i)
        {
            mRumbleEffects[i]->Stop();
            mRumbleEffects[i]->Release();
        }
        mRumbleEffects.clear();
    }
    if(mDevice)
    {
        mDevice->Unacquire(); //does nothing if not acquired
        mDevice->Release();
        mDevice = NULL; //not sure if this is necessary, but it won't hurt.
    }
    return true;
}

void WinControllerDirectInput::Update(const TimeType deltaT)
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

    UpdateRumblers();
}

void WinControllerDirectInput::CheckButtonChange(unsigned int buttonIndex, BYTE oldState, BYTE newState)
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

void WinControllerDirectInput::CheckAxisChange(Axis axisIndex, LONG oldState, LONG newState)
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

void WinControllerDirectInput::CheckPOVChange(unsigned int povIndex, DWORD oldState, DWORD newState)
{
    if(oldState != newState)
    {
        if(POVIsUp(newState) != POVIsUp(oldState)) POVIsUp(newState) ? JoyButtonPressed(povIndex*4) : JoyButtonReleased(povIndex*4);
        if(POVIsDown(newState) != POVIsDown(oldState)) POVIsDown(newState) ? JoyButtonPressed(povIndex*4+1) : JoyButtonReleased(povIndex*4+1);
        if(POVIsLeft(newState) != POVIsLeft(oldState)) POVIsLeft(newState) ? JoyButtonPressed(povIndex*4+2) : JoyButtonReleased(povIndex*4+2);
        if(POVIsRight(newState) != POVIsRight(oldState)) POVIsRight(newState) ? JoyButtonPressed(povIndex*4+3) : JoyButtonReleased(povIndex*4+3);
    }
}

void WinControllerDirectInput::UpdateRumblers()
{
    //for rumbling we need to be in exclusive mode. Also make sure it's enabled.
    if(!(mIsExclusive && mRumbleEnabled)) return;

    assert(mFFAxes.size() >= mNumRumblers);
    for(unsigned int i = 0; i < mNumRumblers; ++i)
    {
        //only update if something changed (significantly).
        if(fabs(mRumbleStrengths[i] - mLastRumbleStrengths[i]) < 0.0001) return;
        mLastRumbleStrengths[i] = mRumbleStrengths[i];

        DIEFFECT effectInfo;
        ZeroMemory(&effectInfo, sizeof(effectInfo));
        effectInfo.dwSize = sizeof(DIEFFECT);

        effectInfo.dwStartDelay = 0;
        effectInfo.dwDuration = INFINITE;
        effectInfo.dwSamplePeriod = 0;
        effectInfo.dwGain = DI_FFNOMINALMAX; //no scaling
        effectInfo.dwTriggerButton = DIEB_NOTRIGGER; //not a button response
        effectInfo.dwTriggerRepeatInterval = 0; //not applicable
        effectInfo.cAxes = 1;
        //DWORD axis = mFFAxes[i];
        effectInfo.rgdwAxes = &mFFAxes[i];
        effectInfo.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        effectInfo.lpEnvelope = 0;
        //long dir = 18000;
        long dir = 0;//DI_FFNOMINALMAX;
        effectInfo.rglDirection = &dir;

        DICONSTANTFORCE forceInfo;
        forceInfo.lMagnitude = Helpers::Round(mRumbleStrengths[i] * DI_FFNOMINALMAX);
        effectInfo.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        effectInfo.lpvTypeSpecificParams = &forceInfo;

        HRESULT result;
        //is there already a rumble effect?
        if(mRumbleEffects.size() <= i)
        {
            mRumbleEffects.push_back(NULL);
            //create
            if((result = mDevice->CreateEffect(/*mRumbleEffectGUID*/ GUID_ConstantForce, &effectInfo, &mRumbleEffects[i], NULL)) != DI_OK)
            {
                std::string err = "Could not create rumble effect for input device \""+mName+"\"! ";
                switch(result)
                {
                    case DIERR_NOTINITIALIZED:
                    {
                        err += "Not initialized.";
                        break;
                    }
                    case DIERR_INVALIDPARAM:
                    {
                        err += "Invalid parameter!";
                        break;
                    }
                    case DIERR_DEVICENOTREG:
                    {
                        err += "Device not registered!";
                        break;
                    }
                    case DIERR_DEVICEFULL:
                    {
                        err += "Device full!";
                        break;
                    }
                    default:
                    {
                        err += "Unhandled error.";
                        break;
                    }
                }
                err += " Not rumbling anymore.";
                Logger::GetDefaultLogger().Warning(err);
                mNumRumblers = 0;
                return;
            }
        }
        //update
        result = mRumbleEffects[i]->SetParameters(&effectInfo, DIEP_TYPESPECIFICPARAMS | DIEP_START);
        if(result != DI_OK && result != DI_EFFECTRESTARTED)
        {
            std::string err = "Could not properly update rumble effect for input device \""+mName+"\"! ";
            switch(result)
            {
                case DI_TRUNCATED:
                case DI_TRUNCATEDANDRESTARTED:
                {
                    err += "Not supported by device.";
                    break;
                }
                case DIERR_NOTINITIALIZED:
                {
                    err += "Not initialized.";
                    break;
                }
                case DIERR_INCOMPLETEEFFECT:
                {
                    err += "Incomplete effect!";
                    break;
                }
                case DIERR_INPUTLOST:
                {
                    err += "Input lost!";
                    break;
                }
                case DIERR_INVALIDPARAM:
                {
                    err += "Invalid parameter!";
                    break;
                }
                case DIERR_EFFECTPLAYING:
                {
                    err += "Effect playing!";
                    break;
                }
                case DI_DOWNLOADSKIPPED:
                {
                    err += "Download skipped!";
                    break;
                }
                default:
                {
                    err += "Unhandled error.";
                    break;
                }
            }
            err += " Not rumbling anymore.";
            Logger::GetDefaultLogger().Warning(err);
            mRumbleEffects[i]->Stop();
            mNumRumblers = 0;
            return;
        }

    }
}

void WinControllerDirectInput::OnFirstWindowCreated(HWND hwnd)
{
    //since we only get this for the first window, this shouldn't be the case.
    assert(!mIsExclusive);

    //there shouldn't be any uninitialized objects, thus this should be set.
    assert(mDevice);

    //DISCL_FOREGROUND means we don't receive messages when the window is in the background. I think that's desirable?
    assert(mDevice->Unacquire() == DI_OK);
    HRESULT result = mDevice->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
    if(result != DI_OK)
    {
        std::string err = "Could not get exclusive access to Input Device \""+mName+"\". ";
        switch(result)
        {
            case DIERR_INVALIDPARAM:
            {
                err += "Invalid Parameter!";
                break;
            }
            case DIERR_NOTINITIALIZED:
            {
                err += "Not initialized!";
                break;
            }
            case E_HANDLE:
            {
                err += "E_HANDLE!";
                break;
            }
            default:
            {
                //I should rather write a Helpers::LongToString method.
                std::stringstream ss;
                ss<<"Unhandled Error "<<result<<"!";
                err += ss.str();
                break;
            }
        }
        err += " Force feedback will be unavailable.";
        Logger::GetDefaultLogger().Warning(err);
    }
    else
    {
        mIsExclusive = true;
    }
    assert(mDevice->Acquire() == DI_OK);
}


std::string WinControllerDirectInput::GetUniqueID() const
{
    std::stringstream ss;
    ss<<mGUID.Data1<<mGUID.Data2<<mGUID.Data3;
    for(int i = 0; i < 8; ++i)
    {
        ss<<mGUID.Data4[i];
    }
    //there may be nulls in the stream, I replace those with 1. (is there a better way to do that? probably.)
    std::stringstream ss2;
    while(true)
    {
        char c;
        ss.get(c);
        if(ss.eof())
        {
            break;
        }
        if(c == '\0')
        {
            c += 1;
        }
        ss2 << c;
    }
    return ss2.str();
}

} // namespace Windows
} // namespace jar
