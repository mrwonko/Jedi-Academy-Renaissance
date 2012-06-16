#include "SimpleLevel.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/core/FileSystem.hpp"
#include "jar/physics/PhysicsWorld.hpp"
#include <algorithm>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <cassert>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h> // for btTriangleIndexVertexArray, my 
#include <BulletDynamics/Dynamics/btDynamicsWorld.h> // for btDynamicsWorld
#include <BulletDynamics/Dynamics/btRigidBody.h> // for btRigidBody
#include <LinearMath/btDefaultMotionState.h> // for btDefaultMotionState
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h> // for btBvhTriangleMeshShape
#include <boost/static_assert.hpp>
#include <cstddef> //offsetof

const char SimpleLevel::IDENT[8] = { 'M', 'R', 'W', 'L', 'V', 'L', '1', '\0' };

namespace
{
    template<typename T, std::size_t len> const std::size_t getArrayLength(const T(&)[len])
    {
        return len;
    }
}

SimpleLevel::SimpleLevel():
    mSurfaces(NULL),
    mUploaded(false)
{
}

SimpleLevel::~SimpleLevel()
{
    delete[] mSurfaces;
    if(mUploaded)
    {
        std::string error;
        if(!DeleteFromGPU(error))
        {
            jar::Logger::GetDefaultLogger().Error("Could not unload SimpleLevel: " + error);
        }
    }
}

const bool SimpleLevel::LoadFromFile(const std::string& filename, std::string& out_error)
{
    jar::fs::GetLastError();

    //  Open for binary reading
    jar::fs::File file = jar::fs::OpenRead(filename);
    if(!file)
    {
        out_error = ("Could not open " + filename + ": " + jar::fs::GetLastError());
        return false;
    }

    //  Read mHeader
    if(jar::fs::Read(file, &mHeader, sizeof(Header), 1) != 1)
    {
        out_error = (filename + " is no valid level file, could not read mHeader: " + jar::fs::GetLastError());
        return false;
    }
    for(std::size_t i = 0; i < getArrayLength(mHeader.ident); ++i)
    {
        if(mHeader.ident[i] != IDENT[i])
        {
            out_error = ( filename + " is no valid level file, ident does not match!");
            return false;
        }
    }
    if(mHeader.version != VERSION)
    {
        out_error = ( "level file " + filename + " has the wrong version (" + jar::Helpers::IntToString(mHeader.version) + " should be " + jar::Helpers::IntToString(VERSION) + ")");
        return false;
    }

    //  Read mSurfaces
    mSurfaces = new Surface[mHeader.numSurfaces];
    for(Surface* curSurf = mSurfaces; curSurf < mSurfaces + mHeader.numSurfaces; ++curSurf)
    {
        if(jar::fs::Read(file, curSurf, sizeof(Surface) - sizeof(curSurf->vertices) - sizeof(curSurf->triangles) - sizeof(curSurf->runtimeData), 1) != 1)
        {
            delete[] mSurfaces;
            mSurfaces = NULL;
            out_error = ("Error reading from level file " + filename + ": " + jar::fs::GetLastError());
            return false;
        }
        curSurf->vertices = new Surface::Vertex[curSurf->numVertices];
        curSurf->triangles = new Surface::Triangle[curSurf->numTriangles];
        if( jar::fs::Read(file, curSurf->vertices, curSurf->numVertices * sizeof(Surface::Vertex), 1) != 1 ||
            jar::fs::Read(file, curSurf->triangles, curSurf->numTriangles * sizeof(Surface::Triangle), 1) != 1)
        {
            delete[] mSurfaces;
            mSurfaces = NULL;
            out_error = ("Error reading from level file " + filename + ": " + jar::fs::GetLastError());
            return false;
        }
        std::string error;
        if(!curSurf->runtimeData.CreatePhysBody(error, *curSurf))
        {
            delete[] mSurfaces;
            mSurfaces = NULL;
            out_error = "Error setting up Physics: " + error;
            return false;
        }
    }

    //  Read mEntities (unknown number)
    enum State
    {
        sStart,
        sInEntity,
        sInKey,
        sPastKey,
        sInValue
    };
    State curState = sStart;
    Entity curEntity;
    std::string curKey;
    std::string curValue;
    while(true)
    {
        if(jar::fs::EndOfFile(file))
        {
            if(curState == sStart)
            {
                return true;
            }
            else
            {
                out_error = ("Unexpected End Of File reading mEntities from " + filename + "!");
                return false;
            }
        }
        char c;
        
        // try reading a character.
        if(!jar::fs::ReadChar(file, c))
        {
            out_error = ("Error reading mEntities from level file " + filename + ": " + jar::fs::GetLastError());
            return false;
        }
        switch(curState)
        {
        case sStart:
            switch(c)
            {
            case ' ':
            case '\t':
            case '\n':
                // ignore whitespaces
                break;
            case '{':
                curState = sInEntity;
                break;
            default:
                out_error = ( "Invalid Syntax in Entity part of level file " + filename + "!");
                return false;
            }
            break;

        case sInEntity:
            switch(c)
            {
            case ' ':
            case '\t':
            case '\n':
                //ignore whitespaces
                break;
            case '"':
                curState = sInKey;
                curKey.clear();
                break;
            case '}':
                if(curEntity.classname.empty())
                {
                    out_error = ( "Warning: Entity without classname in level file " + filename + "!");
                }
                else
                {
                    mEntities.push_back(curEntity);
                }
                curState = sStart;
                break;
            default:
                out_error = ( "Invalid Syntax in Entity part of level file " + filename + "!");
                return false;
            }
            break;

        case sInKey:
            switch(c)
            {
            case '"':
                curState = sPastKey;
                break;
            default:
                curKey += c;
                break;
            }
            break;

        case sPastKey:
            switch(c)
            {
            case ' ':
            case '\t':
            case '\n':
                //ignore whitespaces
                break;
            case '"':
                curState = sInValue;
                curValue.clear();
                break;
            default:
                out_error = ( "Invalid Syntax in Entity part of level file " + filename + "!");
                return false;
            }
            break;

        case sInValue:
            switch(c)
            {
            case '"':
                curState = sInEntity;
                std::transform(curKey.begin(), curKey.end(), curKey.begin(), tolower);
                if(curKey == "classname")
                {
                    if(curEntity.classname.empty())
                    {
                        curEntity.classname = curValue;
                    }
                    else
                    {
                        jar::Logger::GetDefaultLogger().Warning("Duplicate key \"classname\" in Entity in level file " + filename + "!");
                    }
                }
                else
                {
                    if(!curEntity.properties.insert(Entity::PropertyMap::value_type(curKey, curValue)).second)
                    {
                        jar::Logger::GetDefaultLogger().Warning("Duplicate key \"" + curKey + "\" in Entity in level file " + filename + "!");
                    }
                }
                break;
            default:
                curValue += c;
                break;
            }
            break;
        }
        std::string buf;

    }
}

const bool SimpleLevel::UploadToGPU(std::string& out_error)
{
    // Check if there are leftover unhandled opengl errors
    GLenum lastError = glGetError();
    if(lastError != GL_NO_ERROR)
    {
        jar::Logger::GetDefaultLogger().Warning(std::string("g2::Model::UploadToGPU(): Unhandled previous OpenGL error:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
    }

    // Abort if already uploaded
    if(mUploaded)
    {
        out_error = ("Trying to re-upload!");
        return false;
    }

    Surface* end = mSurfaces + mHeader.numSurfaces;
    for(Surface* curSurface = mSurfaces; curSurface != end; ++curSurface)
    {
        if(curSurface->flags & Surface::sfInvisible)
        {
            continue;
        }
        // Generate VBO buffers
        glGenBuffers(2, curSurface->runtimeData.VBOIndices);
        if((lastError = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("Error generating buffers:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
            curSurface->runtimeData.VBOIndices[0] = curSurface->runtimeData.VBOIndices[1] = 0;
            return false;
        }

        //     Upload Data
        //  upload vertices
        //bind buffer
        glBindBuffer(GL_ARRAY_BUFFER, curSurface->runtimeData.VBOIndices[0]);
        if((lastError = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("Error binding buffer:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
                    
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            mUploaded = true; // for deletion
            std::string error;
            if(!DeleteFromGPU(error)) jar::Logger::GetDefaultLogger().Error("SimpleLevel::UploadToGPU(): Could not properly clean up after failed upload: " + error);
            return false;
        }
        //upload data
        glBufferData(GL_ARRAY_BUFFER, sizeof(Surface::Vertex) * curSurface->numVertices, curSurface->vertices, GL_STATIC_DRAW);
        if((lastError = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("Error uploading buffer data:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
                    
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            mUploaded = true; // for deletion
            std::string error;
            if(!DeleteFromGPU(error)) jar::Logger::GetDefaultLogger().Error("SimpleLevel::UploadToGPU(): Could not properly clean up after failed upload: " + error);
            return false;
        }
        
        // unbind buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //  upload triangles
        //bind buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curSurface->runtimeData.VBOIndices[1]);
        if((lastError = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("Error binding buffer:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
                    
            assert(glGetError() == GL_NO_ERROR);
            mUploaded = true; // for deletion
            std::string error;
            if(!DeleteFromGPU(error)) jar::Logger::GetDefaultLogger().Error("SimpleLevel::UploadToGPU(): Could not properly clean up after failed upload: " + error);
            return false;
        }
        //upload data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Surface::Triangle) * curSurface->numTriangles, curSurface->triangles, GL_STATIC_DRAW);
        if((lastError = glGetError()) != GL_NO_ERROR)
        {
            out_error = std::string("Error uploading buffer data:\n") + reinterpret_cast<const char*>(gluErrorString(lastError));
                    
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            assert(glGetError() == GL_NO_ERROR);
            mUploaded = true; // for deletion
            std::string error;
            if(!DeleteFromGPU(error)) jar::Logger::GetDefaultLogger().Error("SimpleLevel::UploadToGPU(): Could not properly clean up after failed upload: " + error);
            return false;
        }
        
        // unbind buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    mUploaded = true;

    return true;
}

const bool SimpleLevel::DeleteFromGPU(std::string& out_error)
{
    if(!mUploaded)
    {
        out_error = ("Not uploaded!");
        return false;
    }

    GLenum err = glGetError();
#ifdef _DEBUG
    if(err != GL_NO_ERROR)
    {
        jar::Logger::GetDefaultLogger().Warning(std::string("SimpleLevel::DeleteFromGPU(): Previously unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
    }
#endif

    bool success = true;

    //glDeleteBuffer calls to 0 are silently ignored
    
    Surface* end = mSurfaces + mHeader.numSurfaces;
    for(Surface* curSurface = mSurfaces; curSurface != end; ++curSurface)
    {
        if(curSurface->flags & Surface::sfInvisible)
        {
            continue;
        }
        glDeleteBuffers(2, curSurface->runtimeData.VBOIndices);
        err = glGetError();
        if(err != GL_NO_ERROR)
        {
            out_error = (std::string("Could not delete triangle VBO buffer: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            success = false;
        }
        curSurface->runtimeData.VBOIndices[0] = curSurface->runtimeData.VBOIndices[1] = 0;
    }

    mUploaded = false;
    return success;
}

const bool SimpleLevel::Render(std::string& out_error)
{
    if(!mUploaded)
    {
        out_error = ("Not uploaded!");
        return false;
    }

    GLenum err = glGetError();
    #ifdef _DEBUG
    if(err != GL_NO_ERROR)
    {
        jar::Logger::GetDefaultLogger().Warning(std::string("SimpleModel::Render(): Previously unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
    }
    #endif
    
    Surface* end = mSurfaces + mHeader.numSurfaces;
    for(Surface* curSurface = mSurfaces; curSurface != end; ++curSurface)
    {
        if(curSurface->flags & Surface::sfInvisible)
        {
            continue;
        }
        glColor3ubv(curSurface->color);

        //   Bind Vertex VBO
        glBindBuffer(GL_ARRAY_BUFFER, curSurface->runtimeData.VBOIndices[0]);
#ifdef _DEBUG
        if((err = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("BindBuffer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            return false;
        }
#endif

        //   Describe memory layout
        // coordinate
        glVertexPointer(3, GL_FLOAT, sizeof(Surface::Vertex), (void*)offsetof(Surface::Vertex, coordinates));
#ifdef _DEBUG
        if((err = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("VertexPointer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return false;
        }
#endif

        // uv
        glTexCoordPointer(2, GL_FLOAT, sizeof(Surface::Vertex), (void*)offsetof(Surface::Vertex, uv));
#ifdef _DEBUG
        if((err = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("TexCoordPointer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return false;
        }
#endif
                
        //   Bind Triangle VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curSurface->runtimeData.VBOIndices[1]);
#ifdef _DEBUG
        if((err = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("BindBuffer (2) OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return false;
        }
#endif

        //   Draw!
        glDrawElements(GL_TRIANGLES, curSurface->numTriangles * 3, GL_UNSIGNED_SHORT, NULL);
#ifdef _DEBUG
        if((err = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("DrawElements OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return false;
        }
#endif
    }
    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if((err = glGetError()) != GL_NO_ERROR)
    {
        out_error = (std::string("Unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
        return false;
    }

    return true;
}

SimpleLevel::Surface::RuntimeData::RuntimeData() :
    physTriangles(NULL),
    physMesh(NULL),
    physCollisionShape(NULL),
    physBody(NULL),
    physMotionState(NULL)
{
    VBOIndices[0] = VBOIndices[1] = 0;
}

SimpleLevel::Surface::RuntimeData::~RuntimeData()
{
    if(physMesh || physBody || physMotionState || physCollisionShape || physTriangles)
    {
        std::string error;
        if(!DeletePhysBody(error))
        {
            jar::Logger::GetDefaultLogger().Error("Could not delete Physical Body of Level Surface: " + error);
        }
    }
    delete[] physTriangles;
}

const bool SimpleLevel::Surface::CreatePhysBody(std::string& out_error)
{
    return runtimeData.CreatePhysBody(out_error, *this);
}

const bool SimpleLevel::Surface::RuntimeData::CreatePhysBody(std::string& out_error, Surface& surface)
{
    if(physMesh || physMotionState || physBody || physCollisionShape || physTriangles)
    {
        out_error = "Physics Body already (partially) created!";
        return false;
    }
    if(!surface.vertices || !surface.triangles)
    {
        out_error = "No valid surface!";
        return false;
    }
    if(!(surface.flags & sfSolid))
    {
        out_error = "Nonsolid surface!";
        return false;
    }
    physTriangles = new int[surface.numTriangles * 3];
    unsigned int index = 0;
    Triangle* endTri = surface.triangles + surface.numTriangles;
    for(Triangle* curTri = surface.triangles; curTri != endTri; ++curTri)
    {
        physTriangles[index++] = curTri->indices[0];
        physTriangles[index++] = curTri->indices[1];
        physTriangles[index++] = curTri->indices[2];
    }
#ifdef BT_USE_DOUBLE_PRECISION
#error Cannot reuse SimpleLevel's vertices when using double precision physics!
#endif
    BOOST_STATIC_ASSERT(sizeof(btScalar) == sizeof(float)); //same as above

    physMesh = new btTriangleIndexVertexArray(surface.numTriangles, physTriangles, sizeof(int), surface.numVertices, reinterpret_cast<float*>(reinterpret_cast<char*>(surface.vertices) + offsetof(Vertex, coordinates)), sizeof(surface.vertices[0]));

    physCollisionShape = new btBvhTriangleMeshShape(physMesh, true); // second parameter: useQuantizedAabbCompression (better memory compression)
    
    //motion states provide a callback for changing an object's position (the rendered one) - I don't need that, so I use a default one.
    physMotionState = new btDefaultMotionState();

    btRigidBody::btRigidBodyConstructionInfo bodyInfo(0.0f, physMotionState, physCollisionShape);
    physBody = new btRigidBody(bodyInfo);
    return true;
}

const bool SimpleLevel::Surface::DeletePhysBody(std::string& out_error)
{
    return runtimeData.DeletePhysBody(out_error);
}

const bool SimpleLevel::Surface::RuntimeData::DeletePhysBody(std::string& out_error)
{
    if(!physBody && !physMesh && !physMotionState && !physCollisionShape && !physTriangles)
    {
        out_error = "No Physics Body created!";
        return false;
    }
    delete physBody;
    physBody = NULL;
    delete physMotionState;
    physMotionState = NULL;
    delete physCollisionShape;
    physCollisionShape = NULL;
    delete physMesh;
    physMesh = NULL;
    delete[] physTriangles;
    physTriangles = NULL;
    return true;
}

const bool SimpleLevel::Surface::AddToPhysWorld(btDynamicsWorld& world, std::string& out_error)
{
    if(!runtimeData.physBody)
    {
        out_error = "No Physics Body created yet!";
        return false;
    }
    world.addRigidBody(runtimeData.physBody);
    return true;
}

const bool SimpleLevel::Surface::DeleteFromPhysWorld(btDynamicsWorld& world, std::string& out_error)
{
    if(!runtimeData.physBody)
    {
        out_error = "No Physics Body created yet!";
        return false;
    }
    world.removeCollisionObject(
    world.removeRigidBody(runtimeData.physBody);
    return true;
}

const bool SimpleLevel::AddToPhysWorld(jar::PhysicsWorld& world, std::string& out_error)
{
    btDynamicsWorld& btWorld = world.GetBulletWorld();
    Surface* surfacesEnd = mSurfaces + mHeader.numSurfaces;
    for(Surface* curSurf = mSurfaces; curSurf != surfacesEnd; ++curSurf)
    {
        if(!(curSurf->flags & Surface::sfSolid))
        {
            continue;
        }
        if(!curSurf->AddToPhysWorld(btWorld, out_error))
        {
            for(--curSurf; curSurf != mSurfaces - 1; --curSurf)
            {
                std::string error;
                if(!curSurf->DeleteFromPhysWorld(btWorld, error))
                {
                    jar::Logger::GetDefaultLogger().Error("SimpleLevel::AddToPhysWorld(): Error during cleanup after failure: " + error);
                }
            }
            return false;
        }
    }
    return true;
}

const bool SimpleLevel::DeleteFromPhysWorld(jar::PhysicsWorld& world, std::string& out_error)
{
    btDynamicsWorld& btWorld = world.GetBulletWorld();
    Surface* surfacesEnd = mSurfaces + mHeader.numSurfaces;
    for(Surface* curSurf = mSurfaces; curSurf != surfacesEnd; ++curSurf)
    {
        if(!(curSurf->flags & Surface::sfSolid))
        {
            continue;
        }
        if(!curSurf->DeleteFromPhysWorld(btWorld, out_error))
        {
            return false;
        }
    }
    return true;
}
