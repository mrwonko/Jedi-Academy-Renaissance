#ifndef JAR_G2_MODEL_HPP
#define JAR_G2_MODEL_HPP

#include <string>
#include <vector>
#include "jar/core/FileSystem.hpp"

struct lua_State;

namespace jar
{

namespace g2
{
    /** \brief Animatable, renderable Ghoul 2 Model (.glm)
    **/
    class Model
    {
    public:
        Model();
        ~Model();
        const bool LoadFromFile(const std::string& filename, std::string& out_error);

        /** \brief Uploads this model to the GPU's vram for fast rendering.
        **/
        const bool UploadToGPU(std::string& out_error);
        /** \brief Deletes this model from the GPU's vram.
        **/
        const bool DeleteFromGPU(std::string& out_error);
        /** \brief Renders this model. Requirement: Has been uploaded to the GPU.
        **/
        const bool Render(std::string& out_error);

        static void BindToLua(lua_State* L);

    private:
        struct SurfaceHierarchyEntry
        {
            const bool LoadFromFile(fs::File file, std::string& out_error);

            std::string name;
            unsigned int flags;
            std::string shader;
            int parentIndex; // -1 if root
            std::vector<int> childIndices; // numChildren entries
        };

        struct Triangle
        {
            const bool LoadFromFile(fs::File file);
            int indices[3];
        };

        struct Vertex
        {
            const bool LoadFromFile(fs::File file);

            float normal[3];                //12 byte
            float coordinate[3];            //12 byte
            float uv[2];                    // 8 Byte
            unsigned char boneIndices[4];   // 4 byte
            unsigned short boneWeights[4];  // 8 byte
                                            //44 byte
        };

        struct Surface
        {
            Surface() : vertexVBOIndex(0), triangleVBOIndex(0) {}
            const bool LoadFromFile(fs::File file, std::string& out_error);

            int index;

            std::vector<int> boneReferences;
            std::vector<Triangle> triangles;
            std::vector<Vertex> vertices;

            unsigned int vertexVBOIndex;
            unsigned int triangleVBOIndex;
        };

        struct LOD
        {
            const bool LoadFromFile(fs::File file, std::string& out_error, const int numSurfaces);

            std::vector<Surface> surfaces;
        };

        std::string mName;
        std::string mAnimName;
        unsigned int mNumBones;
        std::vector<SurfaceHierarchyEntry> mSurfaceHierarchy; //mHeader.numSurfaces entries
        std::vector<SurfaceHierarchyEntry*> mRootSurfaces;
        std::vector<LOD> mLODs; //mHeader.numLODs entries
        bool isUploaded;
    };
}
}

#endif
