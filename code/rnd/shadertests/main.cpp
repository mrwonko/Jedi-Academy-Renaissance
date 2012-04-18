#include <SFML/Graphics.hpp>
#include <iostream>
#include <cassert>
#include <string>
#include <fstream>

#include <gl/glew.h>
#include <gl/gl.h>

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

    //unrelated draw buffer test code
    GLenum drawBuffers[] =
    {
        GL_BACK_LEFT,
        GL_BACK_RIGHT
    };
    assert(glewInit() == GLEW_OK);
    glDrawBuffers(2, drawBuffers);

    int numDrawBuffers = -1;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &numDrawBuffers);
    std::cout<<numDrawBuffers<<" draw buffers supported:" << std::endl;
    for(int i = 0; i < numDrawBuffers; ++i)
    {
        int buffer = -1;
        glGetIntegerv(GL_DRAW_BUFFER0 + i, &buffer);
        switch(buffer)
        {
        case GL_FRONT:
            std::cout<<" GL_FRONT"<<std::endl;
            break;
        case GL_FRONT_LEFT:
            std::cout<<" GL_FRONT_LEFT"<<std::endl;
            break;
        case GL_FRONT_RIGHT:
            std::cout<<" GL_FRONT_RIGHT"<<std::endl;
            break;
        case GL_BACK:
            std::cout<<" GL_BACK"<<std::endl;
            break;
        case GL_BACK_LEFT:
            std::cout<<" GL_BACK_LEFT"<<std::endl;
            break;
        case GL_BACK_RIGHT:
            std::cout<<" GL_BACK_RIGHT"<<std::endl;
            break;
        case GL_NONE:
            std::cout<<" GL_NONE"<<std::endl;
            break;
        default:
            std::cout<<" "<<buffer<< std::endl;
            break;
        }
    }

    //end of unrelated draw buffer testcode

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
