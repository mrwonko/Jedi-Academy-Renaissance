#include <iostream>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include "jar/Event.hpp"
#include "jar/InputDeviceManager.hpp"
#include "jar/Windows/WinKeyboard.hpp"

int main()
{
    std::map<jar::Event::EventType, std::string> eventTypes;
    eventTypes[jar::Event::Closed] = "Closed";
    eventTypes[jar::Event::GainedFocus] = "";
    eventTypes[jar::Event::JoyAxisMoved] = "";
    eventTypes[jar::Event::JoyButtonPressed] = "";
    eventTypes[jar::Event::JoyButtonReleased] = "";
    eventTypes[jar::Event::KeyPressed] = "KeyPressed";
    eventTypes[jar::Event::KeyReleased] = "KeyReleased";
    eventTypes[jar::Event::LostFocus] = "LostFocus";
    eventTypes[jar::Event::MouseButtonPressed] = "MouseButtonPressed";
    eventTypes[jar::Event::MouseButtonReleased] = "MouseButtonReleased";
    eventTypes[jar::Event::MouseMoved] = "MouseMoved";
    //eventTypes[jar::Event::Resized] = "Resized";

    std::map<jar::Key::Code, std::string> keyCodes;
    keyCodes[jar::Key::A] = "A";
    keyCodes[jar::Key::B] = "B";
    keyCodes[jar::Key::C] = "C";
    keyCodes[jar::Key::D] = "D";
    keyCodes[jar::Key::E] = "E";
    keyCodes[jar::Key::F] = "F";
    keyCodes[jar::Key::G] = "G";
    keyCodes[jar::Key::H] = "H";
    keyCodes[jar::Key::I] = "I";
    keyCodes[jar::Key::J] = "J";
    keyCodes[jar::Key::K] = "K";
    keyCodes[jar::Key::L] = "L";
    keyCodes[jar::Key::M] = "M";
    keyCodes[jar::Key::N] = "N";
    keyCodes[jar::Key::O] = "O";
    keyCodes[jar::Key::P] = "P";
    keyCodes[jar::Key::Q] = "Q";
    keyCodes[jar::Key::R] = "R";
    keyCodes[jar::Key::S] = "S";
    keyCodes[jar::Key::T] = "T";
    keyCodes[jar::Key::U] = "U";
    keyCodes[jar::Key::V] = "V";
    keyCodes[jar::Key::W] = "W";
    keyCodes[jar::Key::X] = "X";
    keyCodes[jar::Key::Y] = "Y";
    keyCodes[jar::Key::Z] = "Z";
    keyCodes[jar::Key::Num0] = "Num0";
    keyCodes[jar::Key::Num1] = "Num1";
    keyCodes[jar::Key::Num2] = "Num2";
    keyCodes[jar::Key::Num3] = "Num3";
    keyCodes[jar::Key::Num4] = "Num4";
    keyCodes[jar::Key::Num5] = "Num5";
    keyCodes[jar::Key::Num6] = "Num6";
    keyCodes[jar::Key::Num7] = "Num7";
    keyCodes[jar::Key::Num8] = "Num8";
    keyCodes[jar::Key::Num9] = "Num9";
    keyCodes[jar::Key::Escape] = "Escape";
    keyCodes[jar::Key::LControl] = "LControl";
    keyCodes[jar::Key::LShift] = "LShift";
    keyCodes[jar::Key::LAlt] = "LAlt";
    keyCodes[jar::Key::LSystem] = "LSystem";
    keyCodes[jar::Key::RControl] = "RControl";
    keyCodes[jar::Key::RShift] = "RShift";
    keyCodes[jar::Key::RAlt] = "RAlt";
    keyCodes[jar::Key::RSystem] = "RSystem";
    keyCodes[jar::Key::Menu] = "Menu";
    keyCodes[jar::Key::LBracket] = "LBracket";
    keyCodes[jar::Key::RBracket] = "RBracket";
    keyCodes[jar::Key::SemiColon] = "SemiColon";
    keyCodes[jar::Key::Comma] = "Comma";
    keyCodes[jar::Key::Period] = "Period";
    keyCodes[jar::Key::Quote] = "Quote";
    keyCodes[jar::Key::Slash] = "Slash";
    keyCodes[jar::Key::BackSlash] = "BackSlash";
    keyCodes[jar::Key::Tilde] = "Tilde";
    keyCodes[jar::Key::Equal] = "Equal";
    keyCodes[jar::Key::Dash] = "Dash";
    keyCodes[jar::Key::Space] = "Space";
    keyCodes[jar::Key::Return] = "Return";
    keyCodes[jar::Key::Back] = "Back";
    keyCodes[jar::Key::Tab] = "Tab";
    keyCodes[jar::Key::PageUp] = "PageUp";
    keyCodes[jar::Key::PageDown] = "PageDown";
    keyCodes[jar::Key::End] = "End";
    keyCodes[jar::Key::Home] = "Home";
    keyCodes[jar::Key::Insert] = "Insert";
    keyCodes[jar::Key::Delete] = "Delete";
    keyCodes[jar::Key::Add] = "Add";
    keyCodes[jar::Key::Subtract] = "Subtract";
    keyCodes[jar::Key::Multiply] = "Multiply";
    keyCodes[jar::Key::Divide] = "Divide";
    keyCodes[jar::Key::Left] = "Left";
    keyCodes[jar::Key::Right] = "Right";
    keyCodes[jar::Key::Up] = "Up";
    keyCodes[jar::Key::Down] = "Down";
    keyCodes[jar::Key::Numpad0] = "Numpad0";
    keyCodes[jar::Key::Numpad1] = "Numpad1";
    keyCodes[jar::Key::Numpad2] = "Numpad2";
    keyCodes[jar::Key::Numpad3] = "Numpad3";
    keyCodes[jar::Key::Numpad4] = "Numpad4";
    keyCodes[jar::Key::Numpad5] = "Numpad5";
    keyCodes[jar::Key::Numpad6] = "Numpad6";
    keyCodes[jar::Key::Numpad7] = "Numpad7";
    keyCodes[jar::Key::Numpad8] = "Numpad8";
    keyCodes[jar::Key::Numpad9] = "Numpad9";
    keyCodes[jar::Key::F1] = "F1";
    keyCodes[jar::Key::F2] = "F2";
    keyCodes[jar::Key::F3] = "F3";
    keyCodes[jar::Key::F4] = "F4";
    keyCodes[jar::Key::F5] = "F5";
    keyCodes[jar::Key::F6] = "F6";
    keyCodes[jar::Key::F7] = "F7";
    keyCodes[jar::Key::F8] = "F8";
    keyCodes[jar::Key::F9] = "F9";
    keyCodes[jar::Key::F10] = "F10";
    keyCodes[jar::Key::F11] = "F11";
    keyCodes[jar::Key::F12] = "F12";
    keyCodes[jar::Key::F13] = "F13";
    keyCodes[jar::Key::F14] = "F14";
    keyCodes[jar::Key::F15] = "F15";
    keyCodes[jar::Key::Pause] = "Pause";
    keyCodes[jar::Key::CapsLock] = "CapsLock";
    keyCodes[jar::Key::NumLock] = "NumLock";
    keyCodes[jar::Key::ScrollLock] = "ScrollLock";
    keyCodes[jar::Key::Print] = "Print";
    keyCodes[jar::Key::GER_LT] = "GER_LT";
    keyCodes[jar::Key::Plus] = "Plus";

    jar::InputDeviceManager idm;
    if(!idm.Init())
    {
        std::cout<<"Could not initialize the Input Device Manager!"<<std::endl;
        return 0;
    }
    jar::Windows::WinKeyboard kb;
    if(!kb.Init())
    {
        std::cout<<"Could not initialize the Keyboard!"<<std::endl;
        return 0;
    }
    idm.AddInputDevice(&kb);

    sf::RenderWindow App(sf::VideoMode(800, 600), "SP Pong");

    while(App.IsOpened())
    {
        sf::Event sf_e;
        while(App.GetEvent(sf_e))
        {
            if(sf_e.Type == sf::Event::Closed)
            {
                jar::Event e;
                e.Type = jar::Event::Closed;
                idm.ReceiveEvent(e);
            }
            else if(sf_e.Type == sf::Event::GainedFocus)
            {
                jar::Event e;
                e.Type = jar::Event::GainedFocus;
                idm.ReceiveEvent(e);
            }
            else if(sf_e.Type == sf::Event::LostFocus)
            {
                jar::Event e;
                e.Type = jar::Event::LostFocus;
                idm.ReceiveEvent(e);
            }
        }

        //Event Stuff
        jar::Event e;
        while(idm.GetEvent(e))
        {
            if(eventTypes.find(e.Type) != eventTypes.end())
            {
                std::cout<<"Got "<<eventTypes[e.Type]<<" Event!"<<std::endl;
            }

            switch(e.Type)
            {
            case jar::Event::KeyPressed:
            {
                if(keyCodes.find(e.Key.Code) != keyCodes.end())
                {
                    std::cout<<keyCodes[e.Key.Code]<<std::endl;
                }
                if(e.Key.Code == jar::Key::Escape)
                {
                    App.Close();
                }
                break;
            }
            case jar::Event::KeyReleased:
            {
                if(keyCodes.find(e.Key.Code) != keyCodes.end())
                {
                    std::cout<<keyCodes[e.Key.Code]<<std::endl;
                }
                break;
            }
            case jar::Event::Closed:
            {
                App.Close();
                break;
            }
            default:
                break;
            }
        }

        //Render Stuff
        App.Clear();
        App.Display();
    }
    return 0;
}
