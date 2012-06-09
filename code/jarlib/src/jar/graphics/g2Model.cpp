#include "jar/graphics/g2Model.hpp"
#include "jar/core/Helpers.hpp"
#include <cassert>

namespace jar
{
namespace g2
{
    const char* ModelFile::IDENT = "2LGM";

    const bool ModelFile::Header::LoadFromFile(fs::File& file, std::string& out_error)
    {
        out_error = "Could not read file header!";
        if(fs::Read(file, ident, sizeof(char), 4) != 4) return false;
        if(fs::ReadInt(file, version)) return false;
        if(fs::Read(file, name, sizeof(char), 64) != 64) return false;
        if(fs::Read(file, animName, sizeof(char), 64) != 64) return false;
        if(fs::ReadInt(file, animIndex)) return false;
        if(fs::ReadInt(file, numBones)) return false;
        if(fs::ReadInt(file, numLODs)) return false;
        if(fs::ReadInt(file, ofsLODs)) return false;
        if(fs::ReadInt(file, numSurfaces)) return false;
        if(fs::ReadInt(file, ofsSurfHierarchy)) return false;
        if(fs::ReadInt(file, ofsEnd)) return false;

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
        }

        return true;
    }

    const bool ModelFile::SurfaceHierarchyOffsets::LoadFromFile(fs::File& file, std::string& out_error, const int numSurfaces)
    {
        offsets = new int[numSurfaces];
        for(int i = 0; i < numSurfaces; ++i)
        {
            if(!fs::ReadInt(file, offsets[i]))
            {
                out_error = "Error reading hierarchy offsets: " + fs::GetLastError();
            }
        }
        return true;
    }

    const bool ModelFile::SurfaceHierarchyEntry::LoadFromFile(fs::File& file, std::string& out_error)
    {
        out_error = "Could not read surface hierarchy.";

        if(fs::Read(file, name, sizeof(char), 64) != 64) return false;
        if(!fs::ReadUnsignedInt(file, flags)) return false;
        if(fs::Read(file, shader, sizeof(char), 64) != 64) return false;
        if(!fs::ReadInt(file, shaderIndex)) return false;
        if(!fs::ReadInt(file, parentIndex)) return false;
        if(!fs::ReadInt(file, numChildren)) return false;
        assert(childIndices == NULL);
        childIndices = new int[numChildren];
        for(int i = 0; i < numChildren; ++i)
        {
            if(!fs::ReadInt(file, childIndices[i])) return false;
        }
        return true;
    }

    const bool ModelFile::Triangle::LoadFromFile(fs::File& file)
    {
        if(!fs::ReadInt(file, indices[0])) return false;
        if(!fs::ReadInt(file, indices[1])) return false;
        if(!fs::ReadInt(file, indices[2])) return false;
        return true;
    }

    const bool ModelFile::Vertex::LoadFromFile(fs::File& file)
    {
        if(!fs::ReadFloat(file, normal[0])) return false;
        if(!fs::ReadFloat(file, normal[1])) return false;
        if(!fs::ReadFloat(file, normal[2])) return false;
        if(!fs::ReadFloat(file, coordinate[0])) return false;
        if(!fs::ReadFloat(file, coordinate[1])) return false;
        if(!fs::ReadFloat(file, coordinate[2])) return false;
        if(!fs::ReadUnsignedInt(file, compressedWeightInfo)) return false;
        if(fs::Read(file, BoneWeights, sizeof(unsigned char), 4) != 4) return false;
        return true;
    }

    const bool ModelFile::Surface::LoadFromFile(fs::File& file, std::string& out_error)
    {
        const int baseOffset = fs::Tell(file);

        out_error = "Could not read surface.";
        if(!fs::ReadInt(file, ident)) return false;
        if(!fs::ReadInt(file, index)) return false;
        if(!fs::ReadInt(file, ofsHeader)) return false;
        if(!fs::ReadInt(file, numVerts)) return false;
        if(!fs::ReadInt(file, ofsVerts)) return false;
        if(!fs::ReadInt(file, numTriangles)) return false;
        if(!fs::ReadInt(file, ofsTriangles)) return false;
        if(!fs::ReadInt(file, numBoneReferences)) return false;
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
        assert(boneReferences == NULL);
        boneReferences = new int[numBoneReferences];
        for(int i = 0; i < numBoneReferences; ++i)
        {
            if(!fs::ReadInt(file, boneReferences[i])) return false;
        }

        //  Vertices
        if(!fs::Seek(file, baseOffset + ofsVerts))
        {
            out_error = "Error seeking surface vertices: " + fs::GetLastError();
            return false;
        }
        assert(vertices == NULL);
        vertices = new Vertex[numVerts];
        const Vertex* const endVertices = vertices + numVerts;
        for(Vertex* curVert = vertices; curVert < endVertices; ++curVert)
        {
            if(!curVert->LoadFromFile(file))
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
        assert(triangles == NULL);
        triangles = new Triangle[numTriangles];
        const Triangle* const triangleEnd = triangles + numTriangles;
        for(Triangle* curTriangle = triangles; curTriangle < triangleEnd; ++curTriangle)
        {
            if(!curTriangle->LoadFromFile(file))
            {
                out_error = "Could not read triangle.";
                return false;
            }
        }

        return true;
    }


    const bool ModelFile::LOD::LoadFromFile(fs::File& file, std::string& out_error, const int numSurfaces)
    {
        const int baseOffset = fs::Tell(file);

        if(!fs::ReadInt(file, ofsEnd))
        {
            out_error = "Could not read LODs.";
            return false;
        }

        assert(surfaces == NULL);
        surfaces = new Surface[numSurfaces];
        for(Surface* curSurface = surfaces; curSurface != surfaces + numSurfaces; ++curSurface)
        {
            if(!curSurface->LoadFromFile(file, out_error)) return false;
        }
        if(!fs::Seek(file, baseOffset + ofsEnd))
        {
            out_error = "Could not seek next LOD!";
            return false;
        }
        return true;
    }

    ModelFile::ModelFile() :
        surfaceHierarchy(NULL),
        LODs(NULL)
    {
    }
    
    ModelFile::~ModelFile()
    {
        delete[] surfaceHierarchy;
        delete LODs;
    }

    const bool ModelFile::LoadFromFile(const std::string& filename, std::string& out_error)
    {

        //  open file
        fs::File file = fs::OpenRead(filename);
        if(!file)
        {
            out_error = "Error opening file: " + fs::GetLastError();
           return false;
        }

        //  read header
        if(!header.LoadFromFile(file, out_error)) return false;

        //  read hierarchy
        if(!fs::Seek(file, header.ofsSurfHierarchy))
        {
            out_error = "Error seeking hierarchy offsets: " + fs::GetLastError();
            return false;
        }

        if(!hierarchyOffsets.LoadFromFile(file, out_error, header.numSurfaces)) return false;

        assert(surfaceHierarchy == NULL);
        surfaceHierarchy = new SurfaceHierarchyEntry[header.numSurfaces];
        for(int index = 0; index < header.numSurfaces; ++index)
        {
            if(!fs::Seek(file, header.ofsSurfHierarchy + hierarchyOffsets.offsets[index]))
            {
                out_error = "Error seeking hierarchy offsets: " + fs::GetLastError();
                return false;
            }
            if(!surfaceHierarchy[index].LoadFromFile(file, out_error)) return false;
        }

        // read LODs
        if(!fs::Seek(file, header.ofsLODs))
        {
            out_error = "Error seeking LODs: " + fs::GetLastError();
            return false;
        }
        assert(LODs == NULL);
        LODs = new LOD[header.numLODs];
        for(LOD* curLOD = LODs; curLOD != LODs + header.numLODs; ++curLOD)
        {
            if(!curLOD->LoadFromFile(file, out_error, header.numSurfaces)) return false;
        }

        // since the file need not be in order, we can't sanity check against header.ofsEnd here
    
        return true;
    }

    Model::Model()
    {
    }

    const bool Model::LoadFromFile(const std::string& filename, std::string& out_error)
    {
        ModelFile modelFile;
        if(!modelFile.LoadFromFile(filename, out_error)) return false;

        //uncompress and convert to internal format etc. 
        return true;
    }
}
}
