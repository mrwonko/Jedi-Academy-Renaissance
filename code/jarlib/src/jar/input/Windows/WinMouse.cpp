#include "jar/input/Windows/WinMouse.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"

namespace jar {
namespace Windows {

static WinMouse* g_winMouse = NULL;

static LRESULT CALLBACK InputHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (g_winMouse)
    {
        g_winMouse->InputHook(nCode, wParam, lParam);
        //allow other hooks to process this as well
        return CallNextHookEx(g_winMouse->mInputHook, nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

WinMouse::WinMouse() :
    mInputHook(NULL)
{
    //ctor
    g_winMouse = this;
}

WinMouse::~WinMouse()
{
    //dtor
    if(g_winMouse == this)
    {
        g_winMouse = NULL;
    }
}

const bool WinMouse::Init()
{
    mInputHook = SetWindowsHookEx(WH_MOUSE, &InputHookProc, NULL, GetCurrentThreadId());
    if(!mInputHook)
    {
        Logger::GetDefaultLogger().Error("SingleMouseDevice: Couldn't register hook, Error " + Helpers::ToString(GetLastError()));
        return false;
    }
    return true;
}

const bool WinMouse::Deinit()
{
    if(mInputHook)
    {
        //keep in mind this may fail!
        if(!UnhookWindowsHookEx(mInputHook))
        {
            Logger::GetDefaultLogger().Error("SingleMouseDevice: Couldn't unregister Hook, Error " + Helpers::ToString(GetLastError()));
            return false;
        }
    }
    return true;
}

void WinMouse::InputHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode != HC_ACTION)
    {
        return;
    }

    PMOUSEHOOKSTRUCTEX info = PMOUSEHOOKSTRUCTEX(lParam); //TODO: how to make it send a mousehookstructEX?

    switch(wParam)
    {
        case WM_LBUTTONDOWN:
        {
            MouseButtonPressed(0);
            break;
        }
        case WM_LBUTTONUP:
        {
            MouseButtonReleased(0);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            MouseButtonPressed(1);
            break;
        }
        case WM_RBUTTONUP:
        {
            MouseButtonReleased(1);
            break;
        }
        case WM_MBUTTONDOWN:
        {
            MouseButtonPressed(2);
            break;
        }
        case WM_MBUTTONUP:
        {
            MouseButtonReleased(2);
            break;
        }
        case WM_XBUTTONDOWN:
        {
            if(HIWORD(info->mouseData) & XBUTTON1) MouseButtonPressed(3);
            if(HIWORD(info->mouseData) & XBUTTON2) MouseButtonPressed(4);
            break;
        }
        case WM_XBUTTONUP:
        {
            if(HIWORD(info->mouseData) & XBUTTON1) MouseButtonReleased(3);
            if(HIWORD(info->mouseData) & XBUTTON2) MouseButtonReleased(4);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            MouseWheelMoved(HIWORD(info->mouseData));
            break;
        }
        case WM_MOUSEMOVE:
        {
            int posX = info->pt.x;
            int posY = info->pt.y;
            RECT windowRect;
            GetWindowRect(info->hwnd, &windowRect);
            int windowPosX = windowRect.left;
            int windowPosY = windowRect.top;
            posX -= windowPosX;
            posY -= windowPosY;
            MouseMoved(posX, posY);
        }
        default:
        {
            break;
        }
    }
}

} // namespace Windows
} // namespace jar
