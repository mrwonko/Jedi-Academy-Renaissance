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

#include "jar/input/InputImpl.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"

#ifdef _WIN32
#include "jar/input/Windows/WinKeyboard.hpp"
#include "jar/input/Windows/WinJoystickXInput.hpp"
#include "jar/input/Windows/WinJoystickDirectInput.hpp"

//#include "Wbemidl.h" //for IsXInputDevice
#endif

namespace jar {

InputImpl::InputImpl()
#ifdef _WIN32
    :
    mKeyboard(NULL),
    mDirectInput(NULL)
#endif
{
    //ctor
}

InputImpl::~InputImpl()
{
    //dtor
}

const bool InputImpl::Init()
{
    mInputDeviceManager = new InputDeviceManager();
    if(!mInputDeviceManager || !mInputDeviceManager->Init())
    {
        Deinit();
        return false;
    }

    if(!InitKeyboard())
    {
        Deinit();
        return false;
    }

    if(!InitMouse())
    {
        Deinit();
        return false;
    }

    if(!InitJoysticks())
    {
        Deinit();
        return false;
    }

    return true;
}

const bool InputImpl::InitKeyboard()
{
    #ifdef _WIN32
    mKeyboard = new Windows::WinKeyboard;
    if(!mKeyboard || !mKeyboard->Init())
    {
        Deinit();
        return false;
    }
    mInputDeviceManager->AddInputDevice(mKeyboard);
    #endif
    Logger::GetDefaultLogger().Info("Keyboard initialized!");
    return true;
}

const bool InputImpl::InitMouse()
{
    Logger::GetDefaultLogger().Info("Mouse (not yet) initialized!");
    return true; //for now
}

#ifdef _WIN32
//DirectInput device enumeration callback

//from http://msdn.microsoft.com/en-us/library/ee417014%28v=vs.85%29.aspx

//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput
//-----------------------------------------------------------------------------
static bool IsXInputDevice( const GUID* pGuidProductFromDirectInput ) //TODO fix!
{
#if 0
    IWbemLocator*           pIWbemLocator  = NULL;
    IEnumWbemClassObject*   pEnumDevices   = NULL;
    IWbemClassObject*       pDevices[20]   = {0};
    IWbemServices*          pIWbemServices = NULL;
    BSTR                    bstrNamespace  = NULL;
    BSTR                    bstrDeviceID   = NULL;
    BSTR                    bstrClassName  = NULL;
    DWORD                   uReturned      = 0;
    bool                    bIsXinputDevice= false;
    UINT                    iDevice        = 0;
    VARIANT                 var;
    HRESULT                 hr;

    // CoInit if needed
    hr = CoInitialize(NULL);
    bool bCleanupCOM = SUCCEEDED(hr);

    // Create WMI
    hr = CoCreateInstance( __uuidof(WbemLocator),
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           __uuidof(IWbemLocator),
                           (LPVOID*) &pIWbemLocator);
    if( FAILED(hr) || pIWbemLocator == NULL )
        goto LCleanup;

    bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );if( bstrNamespace == NULL ) goto LCleanup;
    bstrClassName = SysAllocString( L"Win32_PNPEntity" );   if( bstrClassName == NULL ) goto LCleanup;
    bstrDeviceID  = SysAllocString( L"DeviceID" );          if( bstrDeviceID == NULL )  goto LCleanup;

    // Connect to WMI
    hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L,
                                       0L, NULL, NULL, &pIWbemServices );
    if( FAILED(hr) || pIWbemServices == NULL )
        goto LCleanup;

    // Switch security level to IMPERSONATE.
    CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                       RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );

    hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices );
    if( FAILED(hr) || pEnumDevices == NULL )
        goto LCleanup;

    // Loop over all devices
    for( ;; )
    {
        // Get 20 at a time
        hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
        if( FAILED(hr) )
            goto LCleanup;
        if( uReturned == 0 )
            break;

        for( iDevice=0; iDevice<uReturned; iDevice++ )
        {
            // For each device, get its device ID
            hr = pDevices[iDevice]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
            if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL )
            {
                // Check if the device ID contains "IG_".  If it does, then it's an XInput device
				    // This information can not be found from DirectInput
                if( wcsstr( var.bstrVal, L"IG_" ) )
                {
                    // If it does, then get the VID/PID from var.bstrVal
                    DWORD dwPid = 0, dwVid = 0;
                    WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
                    if( strVid && swscanf( strVid, L"VID_%4X", &dwVid ) != 1 )
                        dwVid = 0;
                    WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
                    if( strPid && swscanf( strPid, L"PID_%4X", &dwPid ) != 1 )
                        dwPid = 0;

                    // Compare the VID/PID to the DInput device
                    DWORD dwVidPid = MAKELONG( dwVid, dwPid );
                    if( dwVidPid == pGuidProductFromDirectInput->Data1 )
                    {
                        bIsXinputDevice = true;
                        goto LCleanup;
                    }
                }
            }
            SAFE_RELEASE( pDevices[iDevice] );
        }
    }

LCleanup:
    if(bstrNamespace)
        SysFreeString(bstrNamespace);
    if(bstrDeviceID)
        SysFreeString(bstrDeviceID);
    if(bstrClassName)
        SysFreeString(bstrClassName);
    for( iDevice=0; iDevice<20; iDevice++ )
        SAFE_RELEASE( pDevices[iDevice] );
    SAFE_RELEASE( pEnumDevices );
    SAFE_RELEASE( pIWbemLocator );
    SAFE_RELEASE( pIWbemServices );

    if( bCleanupCOM )
        CoUninitialize();

    return bIsXinputDevice;
#else
    return false;
#endif
}

//data required by the enum function
struct DirectInputEnumInfo
{
    DirectInputEnumInfo(unsigned int& numJoysticks,
                        std::vector<Windows::WinJoystickDirectInput*>& joyVec,
                        InputDeviceManager& manager,
                        LPDIRECTINPUT8 dinput
                        ) :
        NumJoysticks(numJoysticks),
        JoyVec(joyVec),
        DeviceManager(manager),
        DirectInput(dinput)
    {}

    unsigned int& NumJoysticks;
    std::vector<Windows::WinJoystickDirectInput*>& JoyVec;
    InputDeviceManager& DeviceManager;
    LPDIRECTINPUT8 DirectInput;
};

// the __stdcall is very important, and took me ages to figure out...
static __stdcall BOOL ForEachDirectInputDevice(LPCDIDEVICEINSTANCE device, LPVOID voidptr)
{
    //XInput devices are handled separately, so let's ignore them.
    if( IsXInputDevice(&device->guidProduct) )
    {
        //continue enumeration
        return DIENUM_CONTINUE;
    }

    DirectInputEnumInfo* info = static_cast<DirectInputEnumInfo* >(voidptr);

    Windows::WinJoystickDirectInput* joy = new Windows::WinJoystickDirectInput();
    joy->Index = info->NumJoysticks;
    if(!joy->Init(info->DirectInput, device))
    {
        delete joy;
        //we failed once - no reason to believe we will continue to fail. Try next one!
        return DIENUM_CONTINUE;
    }
    info->JoyVec.push_back(joy);
    info->NumJoysticks += 1;
    info->DeviceManager.AddInputDevice(joy);

    //continue enumeration
    return DIENUM_CONTINUE;
}

#endif

const bool InputImpl::InitJoysticks()
{
    unsigned int numJoysticks = 0;

    #ifdef _WIN32

    //  XInput

    XInputEnable(TRUE);
    for(DWORD i = 0; i < 4; ++i)
    {
        XINPUT_STATE state;
        if(XInputGetState(i, &state) == ERROR_SUCCESS)
        {
            //controller connected
            Windows::WinJoystickXInput* joy = new Windows::WinJoystickXInput();
            //InputDeviceJoystick::Index is the JAR-Index of that gamepad, while the one supplied to Init is the XInput index
            joy->Index = numJoysticks;
            if(!joy->Init(i))
            {
                delete joy;
                continue;
            }
            ++numJoysticks;
            mXInputJoysticks.push_back(joy);
            mInputDeviceManager->AddInputDevice(joy);
        }
    }

    //  DirectInput

    HINSTANCE hinst = GetModuleHandle(NULL);
    if(!hinst)
    {
        Logger::GetDefaultLogger().Warning("InputImpl::InitJoysticks(): Could not get module handle! No DirectInput support available.");
        return true;
    }

    if(DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)(&mDirectInput), NULL) != DI_OK)
    {
        Logger::GetDefaultLogger().Warning("InputImpl::InitJoysticks(): Could not create DirectInput interface! No DirectInput support available.");
        return true;
    }

    //info needed in the enum function
    assert(mInputDeviceManager);
    DirectInputEnumInfo info(numJoysticks, mDirectInputJoysticks, *mInputDeviceManager, mDirectInput);
    if(mDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, ForEachDirectInputDevice, static_cast<LPVOID>(&info), DIEDFL_ATTACHEDONLY) != DI_OK)
    {
        Logger::GetDefaultLogger().Warning("InputImpl::InitJoysticks(): Could not enumerate devices! No DirectInput support available.");
        return true;
    }

    #else
    #warning No Joystick/Gamepad support for this OS yet, sorry!
    #endif

    Logger::GetDefaultLogger().Info(Helpers::IntToString(numJoysticks) + " Joystick(s)/Gamepad(s) initialized!");
    return true;
}

const bool InputImpl::Deinit()
{
    bool failed = false;

    if(!DeinitJoysticks())
    {
        failed = true;
    }

    if(!DeinitMouse())
    {
        failed = true;
    }

    if(!DeinitKeyboard())
    {
        failed = true;
    }

    if(mInputDeviceManager && !mInputDeviceManager->Deinit())
    {
        failed = true;
    }
    delete mInputDeviceManager;

    return !failed;
}

const bool InputImpl::DeinitKeyboard()
{
    bool failed = false;
    #ifdef WIN32
    if(mKeyboard && !mKeyboard->Deinit())
    {
        failed = true;
    }
    delete mKeyboard;
    #endif
    return !failed;
}

const bool InputImpl::DeinitMouse()
{
    return true; // for now...
}

const bool InputImpl::DeinitJoysticks()
{
    bool failed = false;

    #ifdef _WIN32

    //  deinit XInput

    XInputEnable(FALSE);
    for(std::vector<Windows::WinJoystickXInput*>::iterator it = mXInputJoysticks.begin(); it != mXInputJoysticks.end(); ++it)
    {
        if(!(*it)->Deinit())
        {
            failed = true;
        }
        delete *it;
    }
    mXInputJoysticks.clear();

    //  deinit DirectInput

    //release joysticks
    for(std::vector<Windows::WinJoystickDirectInput*>::iterator it = mDirectInputJoysticks.begin(); it != mDirectInputJoysticks.end(); ++it)
    {
        if(!(*it)->Deinit())
        {
            failed = true;
        }
        delete *it;
    }
    mDirectInputJoysticks.clear();

    //release DirectInput interface
    if(mDirectInput)
    {
        mDirectInput->Release();
    }
    #endif

    return !failed;
}

void InputImpl::Update(TimeType deltaT)
{
    if(mInputDeviceManager)
    {
        mInputDeviceManager->Update(deltaT);
    }
}

} // namespace jar
