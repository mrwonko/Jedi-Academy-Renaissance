#include "jar/Windows/WinKeyboard.hpp"
#include "jar/KeyCodes.hpp"
#include <iostream>

namespace jar
{
namespace Windows
{

static WinKeyboard* Teh_Keyboardz = NULL;

static LRESULT CALLBACK InputHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (Teh_Keyboardz) Teh_Keyboardz->InputHook(nCode, wParam, lParam);
    //allow other hooks to process this as well
    return CallNextHookEx(Teh_Keyboardz->mInputHook, nCode, wParam, lParam);
}

WinKeyboard::WinKeyboard()
{
    //ctor
    Teh_Keyboardz = this;
    mKeyMap[VK_BACK] = jar::Key::Back;
    mKeyMap[VK_TAB] = jar::Key::Tab;
    mKeyMap[VK_RETURN] = jar::Key::Return;
    mKeyMap[VK_PAUSE] = jar::Key::Pause;
    mKeyMap[VK_CAPITAL] = jar::Key::CapsLock;
    mKeyMap[VK_ESCAPE] = jar::Key::Escape;
    mKeyMap[VK_SPACE] = jar::Key::Space;
    mKeyMap[VK_PRIOR] = jar::Key::PageUp;
    mKeyMap[VK_NEXT] = jar::Key::PageDown;
    mKeyMap[VK_END] = jar::Key::End;
    mKeyMap[VK_HOME] = jar::Key::Home;
    mKeyMap[VK_LEFT] = jar::Key::Left;
    mKeyMap[VK_UP] = jar::Key::Up;
    mKeyMap[VK_DOWN] = jar::Key::Down;
    mKeyMap[VK_LEFT] = jar::Key::Left;
    mKeyMap[VK_RIGHT] = jar::Key::Right;
    //mKeyMap[VK_PRINT] = jar::Key::Print;
    mKeyMap[VK_SNAPSHOT] = jar::Key::Print;
    mKeyMap[VK_INSERT] = jar::Key::Insert;
    mKeyMap[VK_DELETE] = jar::Key::Delete;
    mKeyMap[VK_NUMPAD0] = jar::Key::Num0;
    mKeyMap[VK_NUMPAD1] = jar::Key::Num1;
    mKeyMap[VK_NUMPAD2] = jar::Key::Num2;
    mKeyMap[VK_NUMPAD3] = jar::Key::Num3;
    mKeyMap[VK_NUMPAD4] = jar::Key::Num4;
    mKeyMap[VK_NUMPAD5] = jar::Key::Num5;
    mKeyMap[VK_NUMPAD6] = jar::Key::Num6;
    mKeyMap[VK_NUMPAD7] = jar::Key::Num7;
    mKeyMap[VK_NUMPAD8] = jar::Key::Num8;
    mKeyMap[VK_NUMPAD9] = jar::Key::Num9;
    mKeyMap[VK_MULTIPLY] = jar::Key::Multiply;
    mKeyMap[VK_DIVIDE] = jar::Key::Divide;
    mKeyMap[VK_ADD] = jar::Key::Add;
    mKeyMap[VK_SUBTRACT] = jar::Key::Subtract;
    mKeyMap[VK_NUMLOCK] = jar::Key::NumLock;
    mKeyMap[VK_SCROLL] = jar::Key::ScrollLock;
    mKeyMap[VK_OEM_PLUS] = jar::Key::Plus;
    mKeyMap[VK_OEM_MINUS] = jar::Key::Dash;
    mKeyMap[VK_OEM_COMMA] = jar::Key::Comma;
    mKeyMap[VK_OEM_PERIOD] = jar::Key::Period;
    mKeyMap[VK_OEM_1] = jar::Key::SemiColon;
    mKeyMap[VK_OEM_2] = jar::Key::Slash;
    mKeyMap[VK_OEM_3] = jar::Key::Tilde;
    mKeyMap[VK_OEM_4] = jar::Key::LBracket;
    mKeyMap[VK_OEM_6] = jar::Key::RBracket;
    mKeyMap[VK_OEM_5] = jar::Key::BackSlash;
    mKeyMap[VK_OEM_7] = jar::Key::Quote;
    mKeyMap[VK_OEM_8] = jar::Key::Equal; //I'm not sure this is used?
    mKeyMap[VK_OEM_102] = jar::Key::GER_LT;
    mKeyMap[VK_APPS] = jar::Key::Menu;
    mKeyMap[VK_LWIN] = jar::Key::LSystem;
    mKeyMap[VK_RWIN] = jar::Key::RSystem;
}

WinKeyboard::~WinKeyboard()
{
    //dtor
    if(Teh_Keyboardz == this) Teh_Keyboardz = NULL;
}

const bool WinKeyboard::Init()
{
    mInputHook = SetWindowsHookEx(WH_KEYBOARD, &InputHookProc, NULL, GetCurrentThreadId());
    if(!mInputHook)
    {
        std::cout<< "SingleKeyboardDevice: Couldn't register hook, Error "<<GetLastError()<<std::endl;
        return false;
    }
    return true;
}

void WinKeyboard::Deinit()
{
    if(mInputHook)
    {
        UnhookWindowsHookEx(mInputHook);
    }
}

void WinKeyboard::KeyEvent(const bool released, Key::Code code)
{
    if(released)
    {
        KeyReleased(code);
    }
    else
    {
        KeyPressed(code);
    }
}

// see http://msdn.microsoft.com/en-us/library/ms644984%28v=VS.85%29.aspx
void WinKeyboard::InputHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode < 0)
    {
        //do not process message, this is requested by Win32 api
        return;
    }
    bool released = lParam & 0x80000000;
    //bool wasPressed = lParam & 0x40000000;
    bool isExtended = lParam & 0x1000000; //for alt, shift, control extended means it's the right one.

    if(wParam>= 'A' && wParam <= 'Z')
    {
        KeyEvent(released, Key::Code(wParam - 'A' + 'a'));
        return;
    }
    if(wParam >= '0' && wParam <= '9')
    {
        std::cout<<char(wParam)<<std::endl;
        KeyEvent(released, Key::Code(wParam));
        return;
    }
    if(wParam >= VK_F1 && wParam <= VK_F15)
    {
        KeyEvent(released, Key::Code(wParam - VK_F1 + Key::F1));
        return;
    }
    if(wParam == VK_CONTROL)
    {
        if(!isExtended)
        {
            KeyEvent(released, jar::Key::LControl);
        }
        else
        {
            KeyEvent(released, jar::Key::RControl);
        }
        return;
    }
    if(wParam == VK_SHIFT)
    {
        if(!isExtended)
        {
            KeyEvent(released, jar::Key::LShift);
        }
        else
        {
            KeyEvent(released, jar::Key::RShift);
        }
        return;
    }
    if(wParam == VK_MENU)
    {
        if(!isExtended)
        {
            KeyEvent(released, jar::Key::LAlt);
        }
        else
        {
            KeyEvent(released, jar::Key::RAlt);
        }
        return;
    }
    std::map<WPARAM, jar::Key::Code>::iterator it = mKeyMap.find(wParam);
    if(it != mKeyMap.end())
    {
        KeyEvent(released, it->second);
        return;
    }
    std::cout<<"Unhandled Key Event "<<wParam<<std::endl;
}

} // namespace Windows
} // namespace jar
