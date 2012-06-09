#ifndef JAR_G2_MODEL_HPP
#define JAR_G2_MODEL_HPP

#include <string>
#include "jar/core/FileSystem.hpp"

namespace jar
{

namespace g2
{
    /** \brief The Ghoul 2 Model (.glm) as it is stored in memory

        \note Will be preprocessed for rendering
    **/
    struct ModelFile
    {
        static const int VERSION = 6;
        static const char* IDENT;// Note: Offsets are always relative to the address of the struct they appear in.

        struct Header
        {
            const bool LoadFromFile(fs::File& file, std::string& out_error);

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

        struct SurfaceHierarchyOffsets
        {
            SurfaceHierarchyOffsets() : offsets(NULL) {}
            ~SurfaceHierarchyOffsets() { delete[] offsets; }
            const bool LoadFromFile(fs::File& file, std::string& out_error, const int numSurfaces);

            int* offsets;
        };

        struct SurfaceHierarchyEntry
        {
            SurfaceHierarchyEntry() : childIndices(NULL) {}
            ~SurfaceHierarchyEntry() { delete[] childIndices; }
            const bool LoadFromFile(fs::File& file, std::string& out_error);

            char name[64];
            unsigned int flags;
            char shader[64];
            int shaderIndex; // to be used by code as it pleases
            int parentIndex; // -1 if root
            int numChildren;
            int* childIndices; // numChildren entries
        };

        struct Triangle
        {
            const bool LoadFromFile(fs::File& file);
            int indices[3];
        };

        struct Vertex
        {
            const bool LoadFromFile(fs::File& file);

            float normal[3];
            float coordinate[3];
            unsigned int compressedWeightInfo;
            unsigned char BoneWeights[4];
        };

        struct Surface
        {
            Surface() : boneReferences(NULL), triangles(NULL), vertices(NULL) {}
            ~Surface() { delete[] boneReferences; delete[] triangles; delete[] vertices; }
            const bool LoadFromFile(fs::File& file, std::string& out_error);

            int ident; // to be used by code as it pleases
            int index;
            int ofsHeader; // negative number pointing back to header
            
            int numVerts;
            int ofsVerts;

            int numTriangles;
            int ofsTriangles;

            int numBoneReferences;
            int ofsBoneReferences;

            int* boneReferences;
            Triangle* triangles;
            Vertex* vertices;
        };

        struct LOD
        {
            LOD() : surfaces(NULL) {}
            ~LOD() { delete[] surfaces; }
            const bool LoadFromFile(fs::File& file, std::string& out_error, const int numSurfaces);

            int ofsEnd;
            Surface* surfaces;
        };

        Header header;
        SurfaceHierarchyOffsets hierarchyOffsets;
        SurfaceHierarchyEntry* surfaceHierarchy; //mHeader.numSurfaces entries
        LOD* LODs; //mHeader.numLODs entries

        ModelFile();
        ~ModelFile();
        const bool LoadFromFile(const std::string& filename, std::string& out_error);
    };

    /** \brief Animatable model that can be rendered
    **/

    class Model
    {
    public:
        Model();

        const bool LoadFromFile(const std::string& filename, std::string& out_error);
    };
}
}

#endif
