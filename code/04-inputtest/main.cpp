#include <jar/Core.hpp>
#include <jar/Input.hpp>
#include <jar/input/Event.hpp>
#include <jar/input/InputDeviceManager.hpp>
#include <jar/input/InputDeviceJoystick.hpp>
#include <jar/Core/CoutLogger.hpp>
#include <jar/Core/Helpers.hpp>

#include <SFML/Graphics.hpp>

#include <iostream>

#include <cmath>

int main(int argc, char** argv)
{
    static const float DEADZONE = 0.25f;
    jar::CoutLogger logger;
    logger.Info("Initialized Logger", 0);
    logger.SetLoggingLevel(5);

    //initialize core & other components
    jar::Core core;
    jar::Input input; // just create this once (after core), no need to do anything else (except init core).

    if(!core.Init(argc, argv, "../"))
    {
        return 0;
    }

    jar::InputDeviceManager& idm = input.GetInputDeviceManager();

    sf::RenderWindow App(sf::VideoMode(800, 600), "Input Test(s)");


    sf::Shape gp0_l_stick = sf::Shape::Circle(0, 0, 16, sf::Color::White);
    gp0_l_stick.SetPosition(100, 100);
    sf::String gp0_l_stick_val_x("X: 0");
    sf::String gp0_l_stick_val_y("Y: 0");
    gp0_l_stick_val_x.SetPosition(0, 0);
    gp0_l_stick_val_y.SetPosition(100, 0);
    gp0_l_stick_val_x.SetSize(12);
    gp0_l_stick_val_y.SetSize(12);

    sf::Shape gp0_r_stick = sf::Shape::Circle(0, 0, 16, sf::Color::White);
    gp0_r_stick.SetPosition(300, 100);
    sf::String gp0_r_stick_val_x("X: 0");
    sf::String gp0_r_stick_val_y("Y: 0");
    gp0_r_stick_val_x.SetPosition(200, 0);
    gp0_r_stick_val_y.SetPosition(300, 0);
    gp0_r_stick_val_x.SetSize(12);
    gp0_r_stick_val_y.SetSize(12);


    sf::Shape gp1_l_stick = sf::Shape::Circle(0, 0, 16, sf::Color::White);
    gp1_l_stick.SetPosition(100, 300);
    sf::String gp1_l_stick_val_x("X: 0");
    sf::String gp1_l_stick_val_y("Y: 0");
    gp1_l_stick_val_x.SetPosition(0, 200);
    gp1_l_stick_val_y.SetPosition(100, 200);
    gp1_l_stick_val_x.SetSize(12);
    gp1_l_stick_val_y.SetSize(12);

    sf::Shape gp1_r_stick = sf::Shape::Circle(0, 0, 16, sf::Color::White);
    gp1_r_stick.SetPosition(300, 300);
    sf::String gp1_r_stick_val_x("X: 0");
    sf::String gp1_r_stick_val_y("Y: 0");
    gp1_r_stick_val_x.SetPosition(200, 200);
    gp1_r_stick_val_y.SetPosition(300, 200);
    gp1_r_stick_val_x.SetSize(12);
    gp1_r_stick_val_y.SetSize(12);


    sf::Shape gp2_l_stick = sf::Shape::Circle(0, 0, 16, sf::Color::White);
    gp2_l_stick.SetPosition(500, 100);
    sf::String gp2_l_stick_val_x("X: 0");
    sf::String gp2_l_stick_val_y("Y: 0");
    gp2_l_stick_val_x.SetPosition(400, 0);
    gp2_l_stick_val_y.SetPosition(500, 0);
    gp2_l_stick_val_x.SetSize(12);
    gp2_l_stick_val_y.SetSize(12);

    sf::Shape gp2_r_stick = sf::Shape::Circle(0, 0, 16, sf::Color::White);
    gp2_r_stick.SetPosition(700, 100);
    sf::String gp2_r_stick_val_x("X: 0");
    sf::String gp2_r_stick_val_y("Y: 0");
    gp2_r_stick_val_x.SetPosition(600, 0);
    gp2_r_stick_val_y.SetPosition(700, 0);
    gp2_r_stick_val_x.SetSize(12);
    gp2_r_stick_val_y.SetSize(12);

    //TODO: Show string with current X/Y Values!

    while(App.IsOpened())
    {
        sf::Event sfE;
        while(App.GetEvent(sfE)) //If sf events are not polled, jar events don't work. Problem?
        {
            if(sfE.Type == sf::Event::Closed)
            {
                App.Close();
            }
        }
        jar::Event e;
        while(idm.GetEvent(e))
        {
            switch(e.Type)
            {
                case jar::Event::JoyButtonPressed:
                {
                    std::cout<<"Joystick "<<e.JoyButton.JoyIndex<<" (" << idm.GetJoystick(e.JoyButton.JoyIndex)->GetName() << ") Button "<<e.JoyButton.Button<<" pressed."<<std::endl;
                    break;
                }
                case jar::Event::JoyButtonReleased:
                {
                    std::cout<<"Joystick "<<e.JoyButton.JoyIndex<<" (" << idm.GetJoystick(e.JoyButton.JoyIndex)->GetName() << ") Button "<<e.JoyButton.Button<<" released."<<std::endl;
                    break;
                }
                case jar::Event::JoyAxisMoved:
                {
                    //dead zone
                    //if(fabs(e.JoyAxis.Position) < DEADZONE) break;
                    //float addme = (e.JoyAxis.Position > 0) ? -DEADZONE : DEADZONE;

                    //std::cout<<"Joystick "<<e.JoyAxis.JoyIndex<<" (" << idm.GetJoystick(e.JoyAxis.JoyIndex)->GetName() << ") Axis "<<e.JoyAxis.Axis<<" moved to position "<<(e.JoyAxis.Position + addme) / (1 - DEADZONE) <<"."<<std::endl;
                    //std::cout<<"Joystick "<<e.JoyAxis.JoyIndex<<" (" << idm.GetJoystick(e.JoyAxis.JoyIndex)->GetName() << ") Axis "<<e.JoyAxis.Axis<<" moved to position "<< -e.JoyAxis.Position <<"."<<std::endl;

                    //upper left - XInput
                    if(e.JoyAxis.JoyIndex == 0)
                    {
                        if(e.JoyAxis.Axis == 0)
                        {
                            gp0_l_stick.SetPosition(100+50*e.JoyAxis.Position, gp0_l_stick.GetPosition().y);
                            gp0_l_stick_val_x.SetText("X: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        else if(e.JoyAxis.Axis == 1)
                        {
                            gp0_l_stick.SetPosition(gp0_l_stick.GetPosition().x, 100+50*e.JoyAxis.Position);
                            gp0_l_stick_val_y.SetText("Y: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        if(e.JoyAxis.Axis == 2)
                        {
                            gp0_r_stick.SetPosition(300+50*e.JoyAxis.Position, gp0_r_stick.GetPosition().y);
                            gp0_r_stick_val_x.SetText("X: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        else if(e.JoyAxis.Axis == 3)
                        {
                            gp0_r_stick.SetPosition(gp0_r_stick.GetPosition().x, 100+50*e.JoyAxis.Position);
                            gp0_r_stick_val_y.SetText("Y: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        else if(e.JoyAxis.Axis == 4 || e.JoyAxis.Axis == 5)
                        {
                            idm.GetJoystick(e.JoyAxis.JoyIndex)->SetRumbleStrength(e.JoyAxis.Axis-4, e.JoyAxis.Position);
                        }
                    }
                    //lower left - DI Gamepad
                    if(e.JoyAxis.JoyIndex == 1)
                    {
                        if(e.JoyAxis.Axis == 0)
                        {
                            gp1_l_stick.SetPosition(100+50*e.JoyAxis.Position, gp1_l_stick.GetPosition().y);
                            gp1_l_stick_val_x.SetText("X: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        else if(e.JoyAxis.Axis == 1)
                        {
                            gp1_l_stick.SetPosition(gp1_l_stick.GetPosition().x, 300+50*e.JoyAxis.Position);
                            gp1_l_stick_val_y.SetText("Y: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        if(e.JoyAxis.Axis == 4)
                        {
                            gp1_r_stick.SetPosition(300+50*e.JoyAxis.Position, gp1_r_stick.GetPosition().y);
                            gp1_r_stick_val_x.SetText("X: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        else if(e.JoyAxis.Axis == 5)
                        {
                            gp1_r_stick.SetPosition(gp1_r_stick.GetPosition().x, 300+50*e.JoyAxis.Position);
                            gp1_r_stick_val_y.SetText("Y: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                    }
                    //upper right - DI Joystick
                    if(e.JoyAxis.JoyIndex == 2)
                    {
                        if(e.JoyAxis.Axis == 0)
                        {
                            gp2_l_stick.SetPosition(500+50*e.JoyAxis.Position, gp2_l_stick.GetPosition().y);
                            gp2_l_stick_val_x.SetText("X: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        else if(e.JoyAxis.Axis == 1)
                        {
                            gp2_l_stick.SetPosition(gp2_l_stick.GetPosition().x, 100+50*e.JoyAxis.Position);
                            gp2_l_stick_val_y.SetText("Y: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        if(e.JoyAxis.Axis == 2)
                        {
                            gp2_r_stick.SetPosition(700+50*e.JoyAxis.Position, gp2_r_stick.GetPosition().y);
                            gp2_r_stick_val_x.SetText("X: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }
                        else if(e.JoyAxis.Axis == 3)
                        {
                            gp2_r_stick.SetPosition(gp2_r_stick.GetPosition().x, 100+50*e.JoyAxis.Position);
                            gp2_r_stick_val_y.SetText("Y: " + jar::Helpers::FloatToString(e.JoyAxis.Position));
                        }

                    }
                    break;
                }
                case jar::Event::KeyPressed:
                {
                    std::cout<<"Key "<<e.Key.Code<<" pressed."<<std::endl;
                    if(e.Key.Code == jar::Key::Escape)
                    {
                        App.Close();
                    }
                    break;
                }
                default:
                    break;
            }
        }

        App.Clear(sf::Color(0, 0, 0));

        App.Draw(gp0_l_stick);
        App.Draw(gp0_l_stick_val_x);
        App.Draw(gp0_l_stick_val_y);
        App.Draw(gp0_r_stick);
        App.Draw(gp0_r_stick_val_x);
        App.Draw(gp0_r_stick_val_y);

        App.Draw(gp1_l_stick);
        App.Draw(gp1_l_stick_val_x);
        App.Draw(gp1_l_stick_val_y);
        App.Draw(gp1_r_stick);
        App.Draw(gp1_r_stick_val_x);
        App.Draw(gp1_r_stick_val_y);

        App.Draw(gp2_l_stick);
        App.Draw(gp2_l_stick_val_x);
        App.Draw(gp2_l_stick_val_y);
        //Joystick has only one stick.
        //App.Draw(gp2_r_stick);
        //App.Draw(gp2_r_stick_val_x);
        //App.Draw(gp2_r_stick_val_y);

        App.Display();

        sf::Sleep(0.01f);
        core.Update(10);
    }

    core.Deinit();

    return 0;
}
