#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <gl/glew.h> // before gl.h
#include <gl/gl.h>
#include <gl/glu.h>

int main()
{
    sf::ContextSettings context;
    context.MajorVersion = 2;
    context.MinorVersion = 0;
    context.DepthBits = 24;
    //context.StencilBits = 0;
    context.AntialiasingLevel = 0;

    sf::Window wnd(sf::VideoMode(800, 600), "OpenGL Test", sf::Style::Close, context);

    if(glewInit() != GLEW_OK)
    {
        std::cerr<<"Error: Could not init GLEW"<<std::endl;
        return 0;
    }

    // Set color and depth clear value
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 1.f, 500.f); // I might want to calculate this myself to remove the glu32 library dependency - otoh that's pretty basic.
    glMatrixMode(GL_MODELVIEW);

    sf::Clock c;

    while(wnd.IsOpened())
    {
        sf::Event ev;
        while(wnd.PollEvent(ev))
        {
            if(ev.Type == sf::Event::Closed)
            {
                wnd.Close();
            }
        }
        //if we had multiple windows: wnd.SetActive();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();
        glTranslatef(0.f, 0.f, -200.f);
        glRotatef(c.GetElapsedTime() * 0.05f, 1.f, 0.f, 0.f);
        glRotatef(c.GetElapsedTime() * 0.05f, 0.f, 1.f, 0.f);
        glRotatef(c.GetElapsedTime() * 0.05f, 0.f, 0.f, 1.f);

        glBegin(GL_QUADS);

            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-50.f, -50.f, -50.f);
            glVertex3f(-50.f,  50.f, -50.f);
            glVertex3f( 50.f,  50.f, -50.f);
            glVertex3f( 50.f, -50.f, -50.f);

            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-50.f, -50.f, 50.f);
            glVertex3f(-50.f,  50.f, 50.f);
            glVertex3f( 50.f,  50.f, 50.f);
            glVertex3f( 50.f, -50.f, 50.f);

            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-50.f, -50.f, -50.f);
            glVertex3f(-50.f,  50.f, -50.f);
            glVertex3f(-50.f,  50.f,  50.f);
            glVertex3f(-50.f, -50.f,  50.f);

            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex3f(50.f, -50.f, -50.f);
            glVertex3f(50.f,  50.f, -50.f);
            glVertex3f(50.f,  50.f,  50.f);
            glVertex3f(50.f, -50.f,  50.f);

            glColor3f(0.0f, 1.0f, 1.0f);
            glVertex3f(-50.f, -50.f,  50.f);
            glVertex3f(-50.f, -50.f, -50.f);
            glVertex3f( 50.f, -50.f, -50.f);
            glVertex3f( 50.f, -50.f,  50.f);

            glColor3f(1.0f, 0.0f, 1.0f);
            glVertex3f(-50.f, 50.f,  50.f);
            glVertex3f(-50.f, 50.f, -50.f);
            glVertex3f( 50.f, 50.f, -50.f);
            glVertex3f( 50.f, 50.f,  50.f);

        glEnd();

        wnd.Display();

    }
    return 0;
}
