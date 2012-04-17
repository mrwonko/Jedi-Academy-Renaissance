#include <SFML/Graphics.hpp>
#include <iostream>
#include <cassert>
#include <string>
#include <fstream>

int main()
{
    assert(sf::Shader::isAvailable() && "No shaders available on this system!");
    sf::RenderWindow wnd(sf::VideoMode(800, 600), "Shader tests");

    sf::Shader shader;
    if(!shader.loadFromFile("shader.frag", sf::Shader::Fragment))
    {
        return 0;
    }
    shader.setParameter("UniformStruct.array[0].color", 255, 0, 0);
    shader.setParameter("UniformStruct.array[1].color", 0, 0, 255);

    sf::RectangleShape rect(sf::Vector2f(400, 300));
    rect.setOrigin(200, 150);
    rect.setPosition(400, 300);

    while(wnd.isOpen())
    {
        //  rendering (filling queue)
        wnd.clear();

        sf::RenderStates states;
        states.shader = &shader;
        if(1)
        {
            wnd.draw(rect, states);
        }
        else
        {
            wnd.draw(rect);
        }

        //  logic
        //poll events
        sf::Event ev;
        while(wnd.pollEvent(ev))
        {
            if(ev.type == sf::Event::Closed || (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape))
            {
                wnd.close();
            }
        }


        //  buffer flipping (waits for rendering to finish if not done yet)
        wnd.display();
    }
    return 0;
}
