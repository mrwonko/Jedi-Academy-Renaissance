#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include <cassert>
#include <sstream>
//#include <gl/gl.h>
#include <SFML/OpenGL.hpp> //apparently only contains correct opengl include for each OS
#include <list>
#include <numeric> //accumulate
#include <iomanip>

void DrawQuadTraditional(const float ofsX, const float ofsY, const float sizeX, const float sizeY)
{
    glBegin(GL_QUADS);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(ofsX, ofsY);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(ofsX+sizeX, ofsY);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(ofsX+sizeX, ofsY+sizeY);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(ofsX, ofsY+sizeY);
    glEnd();
}

int main()
{
    enum RenderMode
    {
        RM_TRADITIONAL, // glBegin(); ... glEnd();

        RM_COUNT, //keep last, used to count
    };

    sf::Shader shader;
    assert(sf::Shader::isAvailable() && "No shaders available on this system!");
    if(!shader.loadFromFile("shader.frag", sf::Shader::Fragment))
    {
        return 0;
    }

    sf::Texture background;
    if(!background.loadFromFile("background.jpg"))
    {
        return 0;
    }
    sf::Texture layer1;
    if(!layer1.loadFromFile("layer1.jpg"))
    {
        return 0;
    }
    sf::Texture layer2;
    if(!layer2.loadFromFile("layer2.png"))
    {
        return 0;
    }

    sf::RenderWindow wnd(sf::VideoMode(800, 600), "Shader tests");

    sf::Text fpsText;
    fpsText.setCharacterSize(12);
    sf::Clock timer;
    RenderMode mode = RM_TRADITIONAL;

    // initialize GL

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//    glCullFace(GL_FRONT);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
//    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    //viewport should be correct
    //glViewport(0, 0, wnd.getSize().x, wnd.getSize().y);

    //mean time queues
    static const unsigned int NUM_MEAN_TIMES = 2000;
    std::list<sf::Int64> lastTraditionalTimes;
    std::list<sf::Int64> lastShaderTimes;

    while(wnd.isOpen())
    {
        //  rendering (filling queue)

        //clear screen (both methods work)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        wnd.clear();



        glEnable(GL_TEXTURE_2D);


        //  == draw left rect "traditionally" ==
        //make sure OpenGL is not buffering any commands so we get accurate timings
        glFinish();
        //start measuring time
        timer.restart();

        //background is drawn "normally"
        background.bind(sf::Texture::Normalized);
        glBlendFunc(GL_ONE, GL_ZERO);
        if(mode == RM_TRADITIONAL) DrawQuadTraditional(190, 200, 200, 200);
        //layer 1 is blended additively
        layer1.bind(sf::Texture::Normalized);
        glBlendFunc(GL_ONE, GL_ONE);
        if(mode == RM_TRADITIONAL) DrawQuadTraditional(190, 200, 200, 200);
        //layer 2 is alpha tests
        layer2.bind(sf::Texture::Normalized);
        glBlendFunc(GL_ONE, GL_ZERO);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GEQUAL, 0.5);
        if(mode == RM_TRADITIONAL) DrawQuadTraditional(190, 200, 200, 200);
        glDisable(GL_ALPHA_TEST);

        glFinish();
        //how long did we take?
        sf::Int64 traditionalTime = timer.getElapsedTime().asMicroseconds();

        lastTraditionalTimes.push_back(traditionalTime);
        if(lastTraditionalTimes.size() > NUM_MEAN_TIMES)
        {
            lastTraditionalTimes.pop_front();
        }
        sf::Int64 meanTraditionalTime = std::accumulate(lastTraditionalTimes.begin(), lastTraditionalTimes.end(), 0) / lastTraditionalTimes.size();

        //  == draw right rect with shader ==

        //init
        glDisable(GL_TEXTURE_2D);
        glFinish();

        //start measuring time
        timer.restart();

        shader.bind();
        shader.setParameter("background", background);
        shader.setParameter("layer1", layer1);
        shader.setParameter("layer2", layer2);
        if(mode == RM_TRADITIONAL)
        {
            DrawQuadTraditional(410, 200, 200, 200);
        }
        shader.unbind();

        glFinish();
        //how long did we take?
        sf::Int64 shaderTime = timer.getElapsedTime().asMicroseconds();

        lastShaderTimes.push_back(shaderTime);
        if(lastShaderTimes.size() > NUM_MEAN_TIMES)
        {
            lastShaderTimes.pop_front();
        }
        sf::Int64 meanShaderTime = std::accumulate(lastShaderTimes.begin(), lastShaderTimes.end(), 0) / lastShaderTimes.size();

        //make wnd ready for sfml drawing again
        wnd.pushGLStates();

        //display time taken
        std::stringstream converter;
        converter << std::right;
        converter << "traditional draw time: " << std::setw(7) << traditionalTime << "탎 - average: " << std::setw(7) << meanTraditionalTime << "탎\n";
        converter << "traditional draw time: " << std::setw(7) << shaderTime << "탎 - average: " << std::setw(7) << meanShaderTime << "탎\n";
        converter << "average ratio: " << float(meanTraditionalTime) / float(meanShaderTime) << " or " << float(meanShaderTime) / float(meanTraditionalTime);
        fpsText.setString(converter.str());
        wnd.draw(fpsText);

        //reset opengl to the way it was before sfml messed with it
        wnd.popGLStates();

        //  logic
        //poll events
        sf::Event ev;
        while(wnd.pollEvent(ev))
        {
            if(ev.type == sf::Event::Closed || (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape))
            {
                wnd.close();
            }
            else if(ev.type == sf::Event::KeyPressed)
            {
                switch(ev.key.code)
                {
                case sf::Keyboard::Right:
                    ++*((unsigned int*)&mode);
                    if(mode == RM_COUNT)
                    {
                        *((unsigned int*)&mode) = 0;
                    }
                    break;
                case sf::Keyboard::Left:
                    if(mode == 0)
                    {
                        mode = RM_COUNT;
                    }
                    --*((unsigned int*)&mode);
                default:
                    break;
                }
            }
        }

        //  buffer flipping (waits for rendering to finish if not done yet, i.e. includes glFinish())
        wnd.display();
    }
    return 0;
}
