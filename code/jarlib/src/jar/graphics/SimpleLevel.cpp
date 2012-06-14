#include "jar/graphics/SimpleLevel.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/core/FileSystem.hpp"
#include <algorithm>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <cassert>

namespace jar
{

// "HFPSLVL\0"
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
            Logger::GetDefaultLogger().Error("Could not unload SimpleLevel: " + error);
        }
    }
}

const bool SimpleLevel::LoadFromFile(const std::string& filename, std::string& out_error)
{
    fs::GetLastError();

    //  Open for binary reading
    fs::File file = fs::OpenRead(filename);
    if(!file)
    {
        out_error = ("Could not open " + filename + ": " + fs::GetLastError());
        return false;
    }

    //  Read mHeader
    if(fs::Read(file, &mHeader, sizeof(Header), 1) != 1)
    {
        out_error = (filename + " is no valid level file, could not read mHeader: " + fs::GetLastError());
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
        out_error = ( "level file " + filename + " has the wrong version (" + Helpers::IntToString(mHeader.version) + " should be " + Helpers::IntToString(VERSION) + ")");
        return false;
    }

    //  Read mSurfaces
    mSurfaces = new Surface[mHeader.numSurfaces];
    for(Surface* curSurf = mSurfaces; curSurf < mSurfaces + mHeader.numSurfaces; ++curSurf)
    {
        if(fs::Read(file, curSurf, sizeof(Surface) - sizeof(curSurf->vertices) - sizeof(curSurf->triangles) - sizeof(curSurf->VBOIndices), 1) != 1)
        {
            delete[] mSurfaces;
            out_error = ("Error reading from level file " + filename + ": " + fs::GetLastError());
            return false;
        }
        curSurf->vertices = new Vertex[curSurf->numVertices];
        curSurf->triangles = new Triangle[curSurf->numTriangles];
        if( fs::Read(file, curSurf->vertices, curSurf->numVertices * sizeof(Vertex), 1) != 1 ||
            fs::Read(file, curSurf->triangles, curSurf->numTriangles * sizeof(Triangle), 1) != 1)
        {
            delete[] mSurfaces;
            out_error = ("Error reading from level file " + filename + ": " + fs::GetLastError());
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
        if(fs::EndOfFile(file))
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
        if(!fs::ReadChar(file, c))
        {
            out_error = ("Error reading mEntities from level file " + filename + ": " + fs::GetLastError());
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
                        Logger::GetDefaultLogger().Warning("Duplicate key \"classname\" in Entity in level file " + filename + "!");
                    }
                }
                else
                {
                    if(!curEntity.properties.insert(Entity::PropertyMap::value_type(curKey, curValue)).second)
                    {
                        Logger::GetDefaultLogger().Warning("Duplicate key \"" + curKey + "\" in Entity in level file " + filename + "!");
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
        // Generate VBO buffers
        glGenBuffers(2, curSurface->VBOIndices);
        if((lastError = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("Error generating buffers:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
            curSurface->VBOIndices[0] = curSurface->VBOIndices[1] = 0;
            return false;
        }

        //     Upload Data
        //  upload vertices
        //bind buffer
        glBindBuffer(GL_ARRAY_BUFFER, curSurface->VBOIndices[0]);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * curSurface->numVertices, curSurface->vertices, GL_STATIC_DRAW);
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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curSurface->VBOIndices[1]);
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * curSurface->numTriangles, curSurface->triangles, GL_STATIC_DRAW);
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
        Logger::GetDefaultLogger().Warning(std::string("SimpleLevel::DeleteFromGPU(): Previously unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
    }
#endif

    bool success = true;

    //glDeleteBuffer calls to 0 are silently ignored
    
    Surface* end = mSurfaces + mHeader.numSurfaces;
    for(Surface* curSurface = mSurfaces; curSurface != end; ++curSurface)
    {
        glDeleteBuffers(2, curSurface->VBOIndices);
        err = glGetError();
        if(err != GL_NO_ERROR)
        {
            out_error = (std::string("Could not delete triangle VBO buffer: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            success = false;
        }
        curSurface->VBOIndices[0] = curSurface->VBOIndices[1] = 0;
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
        Logger::GetDefaultLogger().Warning(std::string("SimpleModel::Render(): Previously unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
    }
    #endif
    
    Surface* end = mSurfaces + mHeader.numSurfaces;
    for(Surface* curSurface = mSurfaces; curSurface != end; ++curSurface)
    {
        glColor3ubv(curSurface->color);

        //   Bind Vertex VBO
        glBindBuffer(GL_ARRAY_BUFFER, curSurface->VBOIndices[0]);
#ifdef _DEBUG
        if((err = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("BindBuffer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            return false;
        }
#endif

        //   Describe memory layout
        // coordinate
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, coordinates));
#ifdef _DEBUG
        if((err = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("VertexPointer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return false;
        }
#endif

        // uv
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, uv));
#ifdef _DEBUG
        if((err = glGetError()) != GL_NO_ERROR)
        {
            out_error = (std::string("TexCoordPointer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return false;
        }
#endif
                
        //   Bind Triangle VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curSurface->VBOIndices[1]);
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

}
