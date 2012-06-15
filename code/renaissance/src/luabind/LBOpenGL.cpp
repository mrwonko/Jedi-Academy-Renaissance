#include "jar/graphics/luabind/OpenGl.hpp"
#include "jar/core/Logger.hpp"
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Color.hpp>
#include <LinearMath/btTransform.h>
#include <boost/static_assert.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

namespace jar
{

void Prepare3DRender(const double fovY, const double aspect, const double zNear, const double zFar)
{
#ifdef _DEBUG
    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
    {
        Logger::GetDefaultLogger().Warning(std::string("Prepare3DRender(): Previously unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
    }
#endif
    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST); // enable depth test
    //glDepthMask(GL_TRUE); // enable writing do depth buffer - is enabled by default (?)
    glClearDepth(1.f); // only needs to be done once

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 1.f, 500.f);

    // Clear the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    // setup arrays I will use
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_LIGHTING);

    glEnable(GL_CULL_FACE);

    //   disable any state left over by SFML
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
    //glDisable(GL_TEXTURE_2D);
    glUseProgram(0); // disable shader
    glColor4f(1.f, 1.f, 1.f, 1.f); // reset color
    if(false)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO); // reset blending
    }
    {
        glDisable(GL_BLEND);
    }
    
    // Reset transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //enable usage of glColor*()-settings as Diffuse component
    //glColorMaterial(GL_FRONT, GL_DIFFUSE);
    //glEnable(GL_COLOR_MATERIAL);

#ifdef _DEBUG
    err = glGetError();
    if(err != GL_NO_ERROR)
    {
        Logger::GetDefaultLogger().Warning(std::string("Prepare3DRender(): Unexcpected OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
    }
#endif
}

void Prepare2DRender(sf::RenderTarget& target)
{
    glDisableClientState(GL_VERTEX_ARRAY); // this one's actually used by SFML, too
    glDisableClientState(GL_TEXTURE_COORD_ARRAY); // this one's actually used by SFML, too
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_CULL_FACE);

    target.resetGLStates();
}

void RenderTriangleOfDeath()
{
    static const float SIZE = 20;
    glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 0); glVertex3f( SIZE,  0,    0    );
        glColor3f(0, 1, 0); glVertex3f( 0,     SIZE, SIZE );
        glColor3f(0, 0, 1); glVertex3f( -SIZE, 0,    0    );
    glEnd();
}

namespace gl
{

namespace
{
    void LoadMatrix(float* m)
    {
        glLoadMatrixf(m);
    }

    void LoadMatrix(double* m)
    {
        glLoadMatrixd(m);
    }
}

void LoadTransform(const btTransform& transform)
{
    btScalar matrix[16] =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    transform.getOpenGLMatrix(matrix);
    LoadMatrix(matrix);
}

void LoadIdentity() { glLoadIdentity(); }
void MatrixMode(const GLenum mode) { glMatrixMode(mode); }
void Perspective(const float fovY, const float aspect, const float zNear, const float zFar) { gluPerspective(fovY, aspect, zNear, zFar); }
void Color(const sf::Color& col) { glColor4ub(col.r, col.g, col.b, col.a); }
void PushMatrix() { glPushMatrix(); }
void PopMatrix() { glPopMatrix(); }
void Rotate(const float angle_deg, const float x, const float y, const float z) { glRotatef(angle_deg, x, y, z); }
void Translate(const float x, const float y, const float z) { glTranslatef(x, y, z); }

}

void BindOpenGL(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::def("RenderTriangleOfDeath", &RenderTriangleOfDeath),

        luabind::def("Prepare3DRender", &Prepare3DRender),
        luabind::def("Prepare2DRender", &Prepare2DRender),
        luabind::namespace_("gl")
        [
            luabind::def("LoadTransform", &gl::LoadTransform),
            luabind::def("LoadIdentity", &gl::LoadIdentity),
            luabind::def("MatrixMode", &gl::MatrixMode),
            luabind::def("Perspective", &gl::Perspective),
            luabind::def("Color", &gl::Color),
            luabind::def("PushMatrix", &gl::PushMatrix),
            luabind::def("PopMatrix", &gl::PopMatrix),
            luabind::def("Rotate", &gl::Rotate),
            luabind::def("Translate", &gl::Translate)
        ]
    ];
    luabind::object gl = luabind::globals(L)["jar"]["gl"];
    luabind::object glEnum = luabind::newtable(L);
    gl["enum"] = glEnum;
    glEnum["MODELVIEW"] = GL_MODELVIEW;
    glEnum["PROJECTION"] = GL_PROJECTION;
}

}