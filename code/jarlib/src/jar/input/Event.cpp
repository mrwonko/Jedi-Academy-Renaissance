#include "jar/input/Event.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <map>

namespace jar
{

const bool Event::FromSFML(const sf::Event& e)
{
    static std::map<sf::Keyboard::Key, jar::Key::Code> keyMap;
    if(keyMap.empty())
    {
        for(unsigned long i = sf::Keyboard::A; i <= sf::Keyboard::Z; ++i)
        {
            keyMap[sf::Keyboard::Key(i)] = Key::Code(i - sf::Keyboard::A + Key::A);
        }
        for(unsigned long i = sf::Keyboard::Num0; i <= sf::Keyboard::Num9; ++i)
        {
            keyMap[sf::Keyboard::Key(i)] = Key::Code(i - sf::Keyboard::Num0 + Key::Num0);
        }
        keyMap[sf::Keyboard::Escape] = Key::Escape;
        keyMap[sf::Keyboard::LControl] = Key::LControl;
        keyMap[sf::Keyboard::LShift] = Key::LShift;
        keyMap[sf::Keyboard::LAlt] = Key::LAlt;
        keyMap[sf::Keyboard::LSystem] = Key::LSystem;
        keyMap[sf::Keyboard::RControl] = Key::RControl;
        keyMap[sf::Keyboard::RShift] = Key::RShift;
        keyMap[sf::Keyboard::RAlt] = Key::RAlt;
        keyMap[sf::Keyboard::RSystem] = Key::RSystem;
        keyMap[sf::Keyboard::Menu] = Key::Menu;
        keyMap[sf::Keyboard::LBracket] = Key::LBracket;
        keyMap[sf::Keyboard::RBracket] = Key::RBracket;
        keyMap[sf::Keyboard::SemiColon] = Key::SemiColon;
        keyMap[sf::Keyboard::Comma] = Key::Comma;
        keyMap[sf::Keyboard::Period] = Key::Period;
        keyMap[sf::Keyboard::Quote] = Key::Quote;
        keyMap[sf::Keyboard::Slash] = Key::Slash;
        keyMap[sf::Keyboard::BackSlash] = Key::BackSlash;
        keyMap[sf::Keyboard::Tilde] = Key::Tilde;
        keyMap[sf::Keyboard::Equal] = Key::Equal;
        keyMap[sf::Keyboard::Dash] = Key::Dash;
        keyMap[sf::Keyboard::Space] = Key::Space;
        keyMap[sf::Keyboard::Return] = Key::Return;
        keyMap[sf::Keyboard::Back] = Key::Back;
        keyMap[sf::Keyboard::Tab] = Key::Tab;
        keyMap[sf::Keyboard::PageUp] = Key::PageUp;
        keyMap[sf::Keyboard::PageDown] = Key::PageDown;
        keyMap[sf::Keyboard::End] = Key::End;
        keyMap[sf::Keyboard::Home] = Key::Home;
        keyMap[sf::Keyboard::Insert] = Key::Insert;
        keyMap[sf::Keyboard::Delete] = Key::Delete;
        keyMap[sf::Keyboard::Add] = Key::Add;
        keyMap[sf::Keyboard::Subtract] = Key::Subtract;
        keyMap[sf::Keyboard::Multiply] = Key::Multiply;
        keyMap[sf::Keyboard::Divide] = Key::Divide;
        keyMap[sf::Keyboard::Left] = Key::Left;
        keyMap[sf::Keyboard::Right] = Key::Right;
        keyMap[sf::Keyboard::Up] = Key::Up;
        keyMap[sf::Keyboard::Down] = Key::Down;
        keyMap[sf::Keyboard::Numpad0] = Key::Numpad0;
        keyMap[sf::Keyboard::Numpad1] = Key::Numpad1;
        keyMap[sf::Keyboard::Numpad2] = Key::Numpad2;
        keyMap[sf::Keyboard::Numpad3] = Key::Numpad3;
        keyMap[sf::Keyboard::Numpad4] = Key::Numpad4;
        keyMap[sf::Keyboard::Numpad5] = Key::Numpad5;
        keyMap[sf::Keyboard::Numpad6] = Key::Numpad6;
        keyMap[sf::Keyboard::Numpad7] = Key::Numpad7;
        keyMap[sf::Keyboard::Numpad8] = Key::Numpad8;
        keyMap[sf::Keyboard::Numpad9] = Key::Numpad9;
        keyMap[sf::Keyboard::F1] = Key::F1;
        keyMap[sf::Keyboard::F2] = Key::F2;
        keyMap[sf::Keyboard::F3] = Key::F3;
        keyMap[sf::Keyboard::F4] = Key::F4;
        keyMap[sf::Keyboard::F5] = Key::F5;
        keyMap[sf::Keyboard::F6] = Key::F6;
        keyMap[sf::Keyboard::F7] = Key::F7;
        keyMap[sf::Keyboard::F8] = Key::F8;
        keyMap[sf::Keyboard::F9] = Key::F9;
        keyMap[sf::Keyboard::F10] = Key::F10;
        keyMap[sf::Keyboard::F11] = Key::F11;
        keyMap[sf::Keyboard::F12] = Key::F12;
        keyMap[sf::Keyboard::F13] = Key::F13;
        keyMap[sf::Keyboard::F14] = Key::F14;
        keyMap[sf::Keyboard::F15] = Key::F15;
        keyMap[sf::Keyboard::Pause] = Key::Pause;
    }

    switch(e.type)
    {
        case sf::Event::Closed:
        {
            Type = Event::Closed;
            return true;
        }
        case sf::Event::GainedFocus:
        {
            Type = Event::GainedFocus;
            return true;
        }
        case sf::Event::LostFocus:
        {
            Type = Event::LostFocus;
            return true;
        }
        case sf::Event::KeyPressed:
        {
            if(keyMap.find(e.key.code) != keyMap.end())
            {
                Type = Event::KeyPressed;
                Key.Code = keyMap[e.key.code];
                Key.Alt = e.key.alt;
                Key.Control = e.key.control;
                Key.Shift = e.key.shift;
                return true;
            }
            //else
            return false;
        }
        case sf::Event::KeyReleased:
        {
            if(keyMap.find(e.key.code) != keyMap.end())
            {
                Type = Event::KeyReleased;
                Key.Code = keyMap[e.key.code];
                return true;
            }
            //else
            return false;
        }
        case sf::Event::MouseMoved:
        {
            Type = Event::MouseMoved;
            MouseMove.X = e.mouseMove.x;
            MouseMove.Y = e.mouseMove.y;
            return true;
        }
        case sf::Event::MouseButtonPressed:
        {
            Type = Event::MouseButtonPressed;
            MouseButton.Button = e.mouseButton.button;
            return true;
        }
        case sf::Event::MouseButtonReleased:
        {
            Type = Event::MouseButtonReleased;
            MouseButton.Button = e.mouseButton.button;
            return true;
        }
        case sf::Event::MouseWheelMoved:
        {
            Type = Event::MouseWheelMoved;
            MouseWheel.Delta = e.mouseWheel.delta;
            return true;
        }
        #ifndef _WIN32 //joysticks for win32 handled separately via DirectInput & XInput
        #warning no joystick support on non-win32 yet!
        /*
        case sf::Event::JoyButtonPressed:
        {
            Type = Event::JoyButtonPressed;
            JoyButton.Button = e.joyButton.button;
            JoyButton.JoyIndex = e.joyButton.joystickId;
            return true;
        }
        case sf::Event::JoyButtonReleased:
        {
            Type = Event::JoyButtonReleased;
            JoyButton.Button = e.joyButton.button;
            JoyButton.JoyIndex = e.joyButton.joystickId;
            return true;
        }
        case sf::Event::JoyMoved:
        {
            Type = Event::JoyAxisMoved;
            JoyAxis.Axis = e.joyMove.axis;
            JoyAxis.JoyIndex = e.joyMove.joystickId;
            JoyAxis.Position = e.joyMove.position;
            return true;
        }
        */
        #endif
        default:
        {
            return false;
        }
    }
    return false;
}

}
