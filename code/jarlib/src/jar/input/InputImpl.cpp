#include "jar/input/InputImpl.hpp"
#include "jar/input/InputDeviceManager.hpp"
#include "jar/input/EventManager.hpp"
#include "jar/input/Event.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/core/Lua.hpp"
#include "jar/Core.hpp"
#include "jar/input/InputDeviceJoystick.hpp"

#ifdef _WIN32
#include "jar/input/Windows/WinJoystickXInput.hpp"
#include "jar/input/Windows/WinJoystickDirectInput.hpp"

//for IsXInputDevice
#include <wbemidl.h>
#include <oleauto.h>
// MS's code imports wmsstd.h, thus requiring the entire windows
// media SDK also be installed for a simple macro.  This is
// simpler and less silly.
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//this is only available on msvc, I believe
#ifndef _MSC_VER

#ifndef __uuidof
#define __uuidof(p) &IID_##p // ## is concatenation
#endif

#endif

#endif

#endif

namespace jar {

InputImpl::InputImpl() :
    mInputDeviceManager(NULL),
    mEventManager(NULL)
#ifdef _WIN32
    ,
    mDirectInput(NULL),
    mNoWindowExists(true)
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
    Logger::GetDefaultLogger().Info("- Initializing Input component...", 1);

    mEventManager = new EventManager();
    if(!mEventManager)
    {
        Logger::GetDefaultLogger().Error("Could not allocate Event Manager!");
        return false;
    }
    mInputDeviceManager = new InputDeviceManager();
    if(!mInputDeviceManager || !mInputDeviceManager->Init())
    {
        //no need to call Deinit(), that's done automatically if a component doesn't initialize correctly.
        return false;
    }

    if(!InitJoysticks())
    {
        //no need to call Deinit(), that's done automatically if a component doesn't initialize correctly.
        return false;
    }

    Logger::GetDefaultLogger().Info("Exposing Event System to Lua...", 2);
    lua_State* L = Core::GetSingleton().GetLua().GetState();
    InputDeviceJoystick::Luabind(L);
    InputDeviceManager::Luabind(L);
    Event::Luabind(L);
    InputImpl::Luabind(L);
    Logger::GetDefaultLogger().Info("Exposed Event System to Lua", 2);

    Logger::GetDefaultLogger().Info("- Input component initialized...", 1);

    return true;
}

#ifdef _WIN32

namespace
{
    //DirectInput device enumeration callback

    // from http://pcsx2.googlecode.com/svn/trunk/plugins/LilyPad/DirectInput.cpp / http://stackoverflow.com/questions/4554113/finding-out-whether-a-directinput-device-supports-xinput-with-mingw-gcc

    // Evil code from MS's site.  If only they'd just made a way to get
    // an XInput device's GUID directly in the first place...
    static BOOL IsXInputDevice( const GUID* pGuidProductFromDirectInput )
    {
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
        hr = CoInitialize(NULL); //requires Ole32.lib
        bool bCleanupCOM = SUCCEEDED(hr);

        // Create WMI
        #ifdef _MSC_VER
        REFCLSID theWbemLocatorUUID = __uuidof(WbemLocator);
        REFCLSID theIWbemLocatorUUID = __uuidof(IWbemLocator);
        #else
        //TODO: verify this works on every computer! But I guess the UUID is supposed to be unique.
        //UUIDs read using msvc
        GUID theWbemLocatorUUID;
        theWbemLocatorUUID.Data1 = 1167128593UL;
        theWbemLocatorUUID.Data2 = 7482;
        theWbemLocatorUUID.Data3 = 4560;
        theWbemLocatorUUID.Data4[0] = 137;
        theWbemLocatorUUID.Data4[1] = 31;
        theWbemLocatorUUID.Data4[2] = 0;
        theWbemLocatorUUID.Data4[3] = 170;
        theWbemLocatorUUID.Data4[4] = 0;
        theWbemLocatorUUID.Data4[5] = 75;
        theWbemLocatorUUID.Data4[6] = 46;
        theWbemLocatorUUID.Data4[7] = 36;
        GUID theIWbemLocatorUUID;
        theIWbemLocatorUUID.Data1 = 3692209799UL;
        theIWbemLocatorUUID.Data2 = 29567;
        theIWbemLocatorUUID.Data3 = 4559;
        theIWbemLocatorUUID.Data4[0] = 136;
        theIWbemLocatorUUID.Data4[1] = 77;
        theIWbemLocatorUUID.Data4[2] = 0;
        theIWbemLocatorUUID.Data4[3] = 170;
        theIWbemLocatorUUID.Data4[4] = 0;
        theIWbemLocatorUUID.Data4[5] = 75;
        theIWbemLocatorUUID.Data4[6] = 46;
        theIWbemLocatorUUID.Data4[7] = 36;
        #endif
        hr = CoCreateInstance( theWbemLocatorUUID,
                               NULL,
                               CLSCTX_INPROC_SERVER,
                               theIWbemLocatorUUID,
                               (LPVOID*) &pIWbemLocator);
        if( FAILED(hr) || pIWbemLocator == NULL )
        {
            Logger::GetDefaultLogger().Warning(hr == REGDB_E_CLASSNOTREG ? "IsXInputDevice(): CoCreateInstance() failed! Bad GUID! (Please report this, it's a bug.)" : "IsXInputDevice(): CoCreateInstance() failed!");
            goto LCleanup;
        }

        bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );if( bstrNamespace == NULL ) goto LCleanup; //requires Oleaut32.lib
        bstrClassName = SysAllocString( L"Win32_PNPEntity" );   if( bstrClassName == NULL ) goto LCleanup;
        bstrDeviceID  = SysAllocString( L"DeviceID" );          if( bstrDeviceID == NULL )  goto LCleanup;

        // Connect to WMI
        hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L,
                                           0L, NULL, NULL, &pIWbemServices );
        if( FAILED(hr) || pIWbemServices == NULL )
        {
            Logger::GetDefaultLogger().Warning("IsXInputDevice(): ConnectServer() failed!");
            goto LCleanup;
        }

        // Switch security level to IMPERSONATE.
        CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                           RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );

        hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices );
        if( FAILED(hr) || pEnumDevices == NULL )
        {
            Logger::GetDefaultLogger().Warning("IsXInputDevice(): CreateInstanceEnum() failed!");
            goto LCleanup;
        }

        // Loop over all devices
        for( ;; )
        {
            // Get 20 at a time
            hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
            if( FAILED(hr) )
            {
                Logger::GetDefaultLogger().Warning("IsXInputDevice(): EnumDevices->Next() failed!");
                goto LCleanup;
            }
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
                        if (strVid) {
                            dwVid = wcstoul(strVid+4, 0, 16);
                        }
                        WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
                        if (strPid) {
                            dwPid = wcstoul(strPid+4, 0, 16);
                        }

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
            Logger::GetDefaultLogger().Info("Input Device \""+std::string(device->tszProductName)+"\" is also an XInput device, discarding as DirectInput!", 5);
            return DIENUM_CONTINUE;
        }

        DirectInputEnumInfo* info = static_cast<DirectInputEnumInfo* >(voidptr);

        Windows::WinJoystickDirectInput* joy = new Windows::WinJoystickDirectInput(info->NumJoysticks);
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
            Windows::WinJoystickXInput* joy = new Windows::WinJoystickXInput(numJoysticks);
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
    #endif
    //gamepad support for other OSes is supported using SFML Events. (see Event::FromSFML)

    Logger::GetDefaultLogger().Info(Helpers::IntToString(numJoysticks) + " Joystick(s)/Gamepad(s) initialized!", 1);
    return true;
}

const bool InputImpl::Deinit()
{
    bool failed = false;

    if(!DeinitJoysticks())
    {
        failed = true;
    }

    if(mInputDeviceManager && !mInputDeviceManager->Deinit())
    {
        failed = true;
    }
    delete mInputDeviceManager;
    delete mEventManager;

    return !failed;
}

const bool InputImpl::DeinitJoysticks()
{
    bool failed = false;

    #ifdef _WIN32

    //  deinit XInput

    XInputEnable(FALSE);
    for(std::vector<Windows::WinJoystickXInput*>::iterator it = mXInputJoysticks.begin(); it != mXInputJoysticks.end(); ++it)
    {
        mInputDeviceManager && mInputDeviceManager->RemoveInputDevice(*it); //failure means it hasn't been added yet - that's ok.
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
        mInputDeviceManager && mInputDeviceManager->RemoveInputDevice(*it); //failure means it hasn't been added yet - that's ok.
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

#ifdef _WIN32
namespace
{
    struct EnumThreadWindowsInfo
    {
        EnumThreadWindowsInfo() : Success(false) {}

        HWND Hwnd;
        bool Success;
    };

    static __stdcall BOOL EnumThreadWndProc(HWND hwnd, LPARAM lParam)
    {
        EnumThreadWindowsInfo* info = (EnumThreadWindowsInfo*)lParam;
        info->Hwnd = hwnd;
        info->Success = true;
        return FALSE; //stop enumeration
    }
}

#endif

void InputImpl::OnWindowCreated()
{
#ifdef _WIN32
    if(mNoWindowExists)
    {
        EnumThreadWindowsInfo info;
        EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, (LPARAM)&info);
        if(!info.Success)
        {
            Logger::GetDefaultLogger().Warning("Input::OnWindowCreated() called but no windows were found!");
            return;
        }

        char title[128];
        GetWindowText(info.Hwnd, title, sizeof(title));
        Logger::GetDefaultLogger().Info(std::string("First window created is called \"")+title+"\".", 5);
        for(std::vector<Windows::WinJoystickDirectInput*>::iterator it = mDirectInputJoysticks.begin(); it != mDirectInputJoysticks.end(); ++it)
        {
            (*it)->OnFirstWindowCreated(info.Hwnd);
        }
    }
    mNoWindowExists = false;
#endif
}

} // namespace jar
