#include "jar/graphics/g2Model.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/core/Logger.hpp"
#include <cassert>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <cstddef> //offsetof

namespace jar
{
namespace g2
{
    //

    Model::Model() : 
        isUploaded(false)
    {
    }
    
    Model::~Model()
    {
        if(isUploaded)
        {
            DeleteFromGPU();
        }
    }

    const bool Model::UploadToGPU()
    {
        // Check if there are leftover unhandled opengl errors
        GLenum lastError = glGetError();
        if(lastError != GL_NO_ERROR)
        {
            jar::Logger::GetDefaultLogger().Warning(std::string("g2::Model::UploadToGPU(): Unhandled previous OpenGL error:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
        }

        // Abort if already uploaded
        if(isUploaded)
        {
            jar::Logger::GetDefaultLogger().Error("g2::Model::UploadToGPU(): Trying to re-upload!");
            return false;
        }
        
        std::vector<LOD>::iterator end = mLODs.end();
        for(std::vector<LOD>::iterator curLOD = mLODs.begin(); curLOD != end; ++curLOD)
        {
            std::vector<Surface>::iterator end = curLOD->surfaces.end();
            for(std::vector<Surface>::iterator curSurface = curLOD->surfaces.begin(); curSurface != end; ++curSurface)
            {
                // Generate VBO buffers
                glGenBuffers(1, &curSurface->triangleVBOIndex);
                if((lastError = glGetError()) != GL_NO_ERROR)
                {
                    jar::Logger::GetDefaultLogger().Error(std::string("g2::Model::UploadToGPU(): Error generating buffers:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
                    return false;
                }
                glGenBuffers(1, &curSurface->vertexVBOIndex);
                if((lastError = glGetError()) != GL_NO_ERROR)
                {
                    jar::Logger::GetDefaultLogger().Error(std::string("g2::Model::UploadToGPU(): Error generating buffers:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));

                    isUploaded = true; // for deletion
                    if(!DeleteFromGPU()) jar::Logger::GetDefaultLogger().Error("g2::Modell:UploadToGPU(): Could not properly clean up after failed upload!");
                    return false;
                }

                //     Upload Data
                //  upload vertices
                //bind buffer
                glBindBuffer(GL_ARRAY_BUFFER, curSurface->vertexVBOIndex);
                if((lastError = glGetError()) != GL_NO_ERROR)
                {
                    jar::Logger::GetDefaultLogger().Error(std::string("g2::Model::UploadToGPU(): Error binding buffer:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
                    
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    isUploaded = true; // for deletion
                    if(!DeleteFromGPU()) jar::Logger::GetDefaultLogger().Error("g2::Modell:UploadToGPU(): Could not properly clean up after failed upload!");
                    return false;
                }
                //upload data
                glBufferData(GL_ARRAY_BUFFER, sizeof(Model::Vertex) * curSurface->vertices.size(), curSurface->vertices.data(), GL_STATIC_DRAW);
                if((lastError = glGetError()) != GL_NO_ERROR)
                {
                    jar::Logger::GetDefaultLogger().Error(std::string("g2::Model::UploadToGPU(): Error uploading buffer data:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
                    
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    isUploaded = true; // for deletion
                    if(!DeleteFromGPU()) jar::Logger::GetDefaultLogger().Error("g2::Modell:UploadToGPU(): Could not properly clean up after failed upload!");
                    return false;
                }
        
                // unbind buffer
                glBindBuffer(GL_ARRAY_BUFFER, 0);
        
                //  upload triangles
                //bind buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curSurface->triangleVBOIndex);
                if((lastError = glGetError()) != GL_NO_ERROR)
                {
                    jar::Logger::GetDefaultLogger().Error(std::string("g2::Model::UploadToGPU(): Error binding buffer:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
                    
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    assert(glGetError() == GL_NO_ERROR);
                    isUploaded = true; // for deletion
                    if(!DeleteFromGPU()) jar::Logger::GetDefaultLogger().Error("g2::Modell:UploadToGPU(): Could not properly clean up after failed upload!");
                    return false;
                }
                //upload data
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Model::Vertex) * curSurface->vertices.size(), curSurface->vertices.data(), GL_STATIC_DRAW);
                if((lastError = glGetError()) != GL_NO_ERROR)
                {
                    jar::Logger::GetDefaultLogger().Error(std::string("g2::Model::UploadToGPU(): Error uploading buffer data:\n") + reinterpret_cast<const char*>(gluErrorString(lastError)));
                    
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    assert(glGetError() == GL_NO_ERROR);
                    isUploaded = true; // for deletion
                    if(!DeleteFromGPU()) jar::Logger::GetDefaultLogger().Error("g2::Modell:UploadToGPU(): Could not properly clean up after failed upload!");
                    return false;
                }
        
                // unbind buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }


        isUploaded = true;
        return true;
    }

    const bool Model::DeleteFromGPU()
    {
        // if it's not on the gpu, we silently ignore
        if(!isUploaded)
        {
            jar::Logger::GetDefaultLogger().Error("g2::Model::DeleteFromGPU() called on non-uploaded model!");
            return false;
        }

        GLenum err = glGetError();
#ifdef _DEBUG
        if(err != GL_NO_ERROR)
        {
            Logger::GetDefaultLogger().Warning(std::string("Model::DeleteFromGPU(): Previously unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
        }
#endif

        bool success = true;

        //glDeleteBuffer calls to 0 are silently ignored

        std::vector<LOD>::iterator end = mLODs.end();
        for(std::vector<LOD>::iterator curLOD = mLODs.begin(); curLOD != end; ++curLOD)
        {
            std::vector<Surface>::iterator end = curLOD->surfaces.end();
            for(std::vector<Surface>::iterator curSurface = curLOD->surfaces.begin(); curSurface != end; ++curSurface)
            {
                glDeleteBuffers(1, &curSurface->triangleVBOIndex);
                err = glGetError();
                if(err != GL_NO_ERROR)
                {
                    Logger::GetDefaultLogger().Error(std::string("Could not delete triangle VBO buffer: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
                    success = false;
                }
                glDeleteBuffers(1, &curSurface->vertexVBOIndex);
                err = glGetError();
                if(err != GL_NO_ERROR)
                {
                    Logger::GetDefaultLogger().Error(std::string("Could not delete vertex VBO buffer: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
                    success = false;
                }
                curSurface->triangleVBOIndex = 0;
                curSurface->vertexVBOIndex = 0;
            }
        }

        isUploaded = false;
        return success;
    }

    const bool Model::Render()
    {
        if(!isUploaded)
        {
            jar::Logger::GetDefaultLogger().Error("g2::Model::Render() called on non-uploaded model!");
            return false;
        }

        GLenum err = glGetError();
#ifdef _DEBUG
        if(err != GL_NO_ERROR)
        {
            Logger::GetDefaultLogger().Warning(std::string("Model::Render(): Previously unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
        }
#endif
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        
        std::vector<LOD>::iterator end = mLODs.end();
        for(std::vector<LOD>::iterator curLOD = mLODs.begin(); curLOD != end; ++curLOD)
        {
            std::vector<Surface>::iterator end = curLOD->surfaces.end();
            for(std::vector<Surface>::iterator curSurface = curLOD->surfaces.begin(); curSurface != end; ++curSurface)
            {
                glBindBuffer(GL_ARRAY_BUFFER, curSurface->vertexVBOIndex);
#ifdef _DEBUG
                if((err = glGetError()) != GL_NO_ERROR)
                {
                    Logger::GetDefaultLogger().Error(std::string("Model::Render(): BindBuffer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
                    return false;
                }
#endif

                // coordinate
                glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, coordinate));
#ifdef _DEBUG
                if((err = glGetError()) != GL_NO_ERROR)
                {
                    Logger::GetDefaultLogger().Error(std::string("Model::Render(): VertexPointer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
                    return false;
                }
#endif

                // normal
                glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
#ifdef _DEBUG
                if((err = glGetError()) != GL_NO_ERROR)
                {
                    Logger::GetDefaultLogger().Error(std::string("Model::Render(): NormalPointer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
                    return false;
                }
#endif

                // uv
                glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, uv));
#ifdef _DEBUG
                if((err = glGetError()) != GL_NO_ERROR)
                {
                    Logger::GetDefaultLogger().Error(std::string("Model::Render(): TexCoordPointer OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
                    return false;
                }
#endif

                // todo: bind skinning attributes
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curSurface->triangleVBOIndex);
#ifdef _DEBUG
                if((err = glGetError()) != GL_NO_ERROR)
                {
                    Logger::GetDefaultLogger().Error(std::string("Model::Render(): BindBuffer (2) OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
                    return false;
                }
#endif

                glDrawElements(GL_TRIANGLES, curSurface->triangles.size() * 3, GL_UNSIGNED_INT, NULL);
#ifdef _DEBUG
                if((err = glGetError()) != GL_NO_ERROR)
                {
                    Logger::GetDefaultLogger().Error(std::string("Model::Render(): DrawElements OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
                    return false;
                }
#endif
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        if((err = glGetError()) != GL_NO_ERROR)
        {
            Logger::GetDefaultLogger().Error(std::string("Model::Render(): Unhandled OpenGL error: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));
            return false;
        }

        return true;
    }

    // == File Format specific structures ==
    static const int VERSION = 6;
    static const char* IDENT = "2LGM";

    // Note: Offsets are always relative to the address of the struct they appear in.
    struct ModelHeader
    {
        const bool LoadFromFile(fs::File file, std::string& out_error);

        char ident[4]; //"2LGM"
        int version;
        char name[64];
        char animName[64];
        int animIndex; // to be used by code as it pleases

        int numBones; // for compatibility checks

        int numLODs;
        int ofsLODs;

        int numSurfaces;
        int ofsSurfHierarchy;

        int ofsEnd; // = file size
    };

    struct ModelSurfaceOffsets
    {
        const bool LoadFromFile(fs::File file, std::string& out_error, const int numSurfaces);

        std::vector<int> offsets;
    };

    // == Helper functions ==

    // Quake style char[64] string (need not end in NULL) to std::string
    static std::string QuakeStringToString(const char* const str)
    {
        std::string result(str, 64);
        std::string::size_type end = result.find_first_of('\0');
        if(end != std::string::npos)
        {
            result.erase(end);
        }
        return result;
    }

    // == File Loading Functions ==

    const bool ModelHeader::LoadFromFile(fs::File file, std::string& out_error)
    {
        //  clear last error
        fs::GetLastError();
        if( fs::Read(file, ident, sizeof(char), 4) != 4 ||
            !fs::ReadInt(file, version) ||
            fs::Read(file, name, sizeof(char), 64) != 64 ||
            fs::Read(file, animName, sizeof(char), 64) != 64 ||
            !fs::ReadInt(file, animIndex) ||
            !fs::ReadInt(file, numBones)||
            !fs::ReadInt(file, numLODs) ||
            !fs::ReadInt(file, ofsLODs) ||
            !fs::ReadInt(file, numSurfaces) ||
            !fs::ReadInt(file, ofsSurfHierarchy) ||
            !fs::ReadInt(file, ofsEnd) )
        {
            out_error = "Could not read file header. " + fs::GetLastError();
            return false;
        }

        // Check ident
        if(std::string(ident, 4) != std::string(IDENT, 4))
        {
            out_error = "No valid Ghoul2 model: Invalid identifier";
            return false;
        }
        // Check version
        if(version != VERSION)
        {
            out_error = "No valid Ghoul2 model: Invalid version";
            return false;
        }

        if(numBones < 0 || numLODs < 0 || numSurfaces < 0)
        {
            out_error = "Invalid values.";
            return false;
        }

        return true;
    }

    const bool ModelSurfaceOffsets::LoadFromFile(fs::File file, std::string& out_error, const int numSurfaces)
    {
        offsets.resize(numSurfaces);
        for(int i = 0; i < numSurfaces; ++i)
        {
            if(!fs::ReadInt(file, offsets[i]))
            {
                out_error = "Error reading hierarchy offsets. " + fs::GetLastError();
                return false;
            }
        }
        return true;
    }

    const bool Model::SurfaceHierarchyEntry::LoadFromFile(fs::File file, std::string& out_error)
    {
        //  clear last error
        fs::GetLastError();
        char qname[64];
        char qshader[64];
        int shaderIndex;
        int numChildren;

        if( fs::Read(file, qname, sizeof(char), 64) != 64 ||
            !fs::ReadUnsignedInt(file, flags) ||
            fs::Read(file, qshader, sizeof(char), 64) != 64 ||
            !fs::ReadInt(file, shaderIndex) ||
            !fs::ReadInt(file, parentIndex) ||
            !fs::ReadInt(file, numChildren))
        {
            out_error = "Could not read surface hierarchy. " + fs::GetLastError() + (fs::EndOfFile(file) ? "\n(End of file)" : "");
            return false;
        }
        name = QuakeStringToString(qname);
        shader = QuakeStringToString(qshader);
        childIndices.resize(numChildren);
        for(int i = 0; i < numChildren; ++i)
        {
            if(!fs::ReadInt(file, childIndices[i]))
            {
                return false;
            }
        }
        return true;
    }

    const bool Model::Triangle::LoadFromFile(fs::File file)
    {
        if(!fs::ReadInt(file, indices[0])) return false;
        if(!fs::ReadInt(file, indices[1])) return false;
        if(!fs::ReadInt(file, indices[2])) return false;
        return true;
    }

    const bool Model::Vertex::LoadFromFile(fs::File file)
    {
        if(!fs::ReadFloat(file, normal[0])) return false;
        if(!fs::ReadFloat(file, normal[1])) return false;
        if(!fs::ReadFloat(file, normal[2])) return false;
        if(!fs::ReadFloat(file, coordinate[0])) return false;
        if(!fs::ReadFloat(file, coordinate[1])) return false;
        if(!fs::ReadFloat(file, coordinate[2])) return false;

        // weight & index are compressed:
        unsigned int compressedWeightInfo;
        if(!fs::ReadUnsignedInt(file, compressedWeightInfo)) return false;
        unsigned char rawBoneWeights[4];
        if(fs::Read(file, rawBoneWeights, sizeof(unsigned char), 4) != 4) return false;

        // bit 30 and 31 are weight count
        unsigned int numWeights = (compressedWeightInfo >> 30) + 1;

        // indices are 5 bit each, index into the boneReferences (up to 32)
        // weights are 10 bit each - 8 bit in rawBoneWeights, 2 in compressedWeightInfo
        short totalWeight = 0;
        static const short MAX_WEIGHT = (1 << 10) - 1;
        for(unsigned int i = 0; i < 4; ++i)
        {
            //bottom 20 bits are 4 indices of 5 bit each
            boneIndices[i] = (compressedWeightInfo >> (5 * i)) & ((1 << 5) - 1);
            short weight = 0;
            if(i < numWeights - 1)
            {
                // bit 20-27 are 2 bit weight overflow
                // shift by 20 - 8 = 12 (8 because sizeof(rawBoneWeights[0]) == 8)
                weight = rawBoneWeights[i] | ((compressedWeightInfo >> (12 + (2 * i))) & (0x300));
                totalWeight += weight;
            }
            else if(i == numWeights - 1)
            {
                weight = MAX_WEIGHT - totalWeight;
            }
            else
            {
                boneIndices[i] = 0;
            }
            // shift weight so it uses the whole short range and can be properly normalized
            boneWeights[i] = weight << (sizeof(unsigned short) * 8 - 10);
        }

        return true;
    }

    const bool Model::Surface::LoadFromFile(fs::File file, std::string& out_error)
    {
        const int baseOffset = fs::Tell(file);

        out_error = "Could not read surface.";
        int ident; // used by original code
        if(!fs::ReadInt(file, ident)) return false;
        if(!fs::ReadInt(file, index)) return false;
        int ofsHeader; // negative number pointing back to header
        if(!fs::ReadInt(file, ofsHeader)) return false;
        int numVerts;
        if(!fs::ReadInt(file, numVerts)) return false;
        int ofsVerts;
        if(!fs::ReadInt(file, ofsVerts)) return false;
        int numTriangles;
        if(!fs::ReadInt(file, numTriangles)) return false;
        int ofsTriangles;
        if(!fs::ReadInt(file, ofsTriangles)) return false;
        int numBoneReferences;
        if(!fs::ReadInt(file, numBoneReferences)) return false;
        int ofsBoneReferences;
        if(!fs::ReadInt(file, ofsBoneReferences)) return false;

        if(ofsHeader != -baseOffset)
        {
            out_error = "Surface file position missmatch.";
            return false;
        }

        //  Bone References
        if(!fs::Seek(file, baseOffset + ofsBoneReferences))
        {
            out_error = "Error seeking surface bone references: " + fs::GetLastError();
            return false;
        }
        boneReferences.resize(numBoneReferences);
        std::vector<int>::iterator end = boneReferences.end();
        for(int i = 0; i < numBoneReferences; ++i)
        {
            if(!fs::ReadInt(file, boneReferences[i]))
            {
                out_error = "Error reading bone references.";
                return false;
            }
        }

        //  Vertices
        if(!fs::Seek(file, baseOffset + ofsVerts))
        {
            out_error = "Error seeking surface vertices: " + fs::GetLastError();
            return false;
        }
        vertices.resize(numVerts);
        std::vector<Vertex>::iterator endVertices = vertices.end();
        for(std::vector<Vertex>::iterator curVert = vertices.begin(); curVert != endVertices; ++curVert)
        {
            if(!curVert->LoadFromFile(file))
            {
                out_error = "Could not read vertex.";
                return false;
            }
        }
        //UV Coordinates come after the other vertex information, for some reason.
        for(std::vector<Vertex>::iterator curVert = vertices.begin(); curVert != endVertices; ++curVert)
        {
            if( !fs::ReadFloat(file, curVert->uv[0]) ||
                !fs::ReadFloat(file, curVert->uv[1]) )
            {
                out_error = "Could not read vertex.";
                return false;
            }
        }

        //  Triangles
        if(!fs::Seek(file, baseOffset + ofsTriangles))
        {
            out_error = "Error seeking surface triangles: " + fs::GetLastError();
            return false;
        }
        triangles.resize(numTriangles);
        std::vector<Triangle>::iterator triangleEnd = triangles.end();
        for(std::vector<Triangle>::iterator curTriangle = triangles.begin(); curTriangle != triangleEnd; ++curTriangle)
        {
            if(!curTriangle->LoadFromFile(file))
            {
                out_error = "Could not read triangle.";
                return false;
            }
        }

        return true;
    }

    const bool Model::LOD::LoadFromFile(fs::File file, std::string& out_error, const int numSurfaces)
    {
        const int baseOffset = fs::Tell(file);
        int ofsEnd;


        if(!fs::ReadInt(file, ofsEnd))
        {
            out_error = "Could not read LOD's ofsEnd.";
            return false;
        }

        ModelSurfaceOffsets surfaceOffsets;
        if(!surfaceOffsets.LoadFromFile(file, out_error, numSurfaces))
        {
            return false;
        }

        surfaces.resize(numSurfaces);

        std::vector<Surface>::iterator end = surfaces.end();
        for(int index = 0; index < numSurfaces; ++index)
        {
            if(!fs::Seek(file, baseOffset + surfaceOffsets.offsets[index] + 4)) // 4 is the size of ofsEnd
            {
                out_error = "Could not seek surface " + Helpers::IntToString(index) + "!";
                return false;
            }
            if(!surfaces[index].LoadFromFile(file, out_error)) return false;
            if(surfaces[index].index != index)
            {
                out_error = "Surface index mismatch!";
                return false;
            }
        }
        if(!fs::Seek(file, baseOffset + ofsEnd))
        {
            out_error = "Could not seek next LOD!";
            return false;
        }
        return true;
    }

    const bool Model::LoadFromFile(const std::string& filename, std::string& out_error)
    {
        //  clear last error
        fs::GetLastError();
        //  open file
        fs::File file = fs::OpenRead(filename);
        if(file == NULL)
        {
            out_error = "Error opening file: " + fs::GetLastError();
            return false;
        }
        
        //  read header
        ModelHeader header;
        if(!header.LoadFromFile(file, out_error)) return false;

        mName = QuakeStringToString(header.name);
        mAnimName = QuakeStringToString(header.animName);

        //  read hierarchy - always directly after header, /not/ at header.ofsSurfHierarchy, that's the first entry's position!

        int surfaceHierarchyBaseOffset = fs::Tell(file);

        ModelSurfaceOffsets hierarchyOffsets;
        //Logger::GetDefaultLogger().Log("There are " + Helpers::IntToString(header.numSurfaces) + " surfaces");
        if(!hierarchyOffsets.LoadFromFile(file, out_error, header.numSurfaces)) return false;

        //read hierarchy
        mSurfaceHierarchy.resize(header.numSurfaces);
        for(int index = 0; index < header.numSurfaces; ++index)
        {
            if(!fs::Seek(file, surfaceHierarchyBaseOffset + hierarchyOffsets.offsets[index]))
            {
                out_error = "Error seeking hierarchy offsets: " + fs::GetLastError();
                return false;
            }
            if(!mSurfaceHierarchy[index].LoadFromFile(file, out_error)) return false;
            //remember root surfaces for hierarchical rendering
            if(mSurfaceHierarchy[index].parentIndex == -1)
            {
                mRootSurfaces.push_back(&mSurfaceHierarchy[index]);
            }
        }
        if(mRootSurfaces.size() == 0)
        {
            out_error = "Broken hierarchy!";
            return false;
        }

        //  read LODs
        if(!fs::Seek(file, header.ofsLODs))
        {
            out_error = "Error seeking LODs: " + fs::GetLastError();
            return false;
        }
        mLODs.resize(header.numLODs);
        std::vector<LOD>::iterator LODsEnd = mLODs.end();
        for(std::vector<LOD>::iterator curLOD = mLODs.begin(); curLOD != LODsEnd; ++curLOD)
        {
            if(!curLOD->LoadFromFile(file, out_error, header.numSurfaces)) return false;
        }

        // since the file need not be in order, we can't sanity check against header.ofsEnd here
    
        return true;
    }
}
}
