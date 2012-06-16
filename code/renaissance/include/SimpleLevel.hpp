#ifndef JAR_GRAPHICS_SIMPLE_LEVEL_HPP
#define JAR_GRAPHICS_SIMPLE_LEVEL_HPP

#include <string>
#include <map>
#include <list>

struct lua_State;
class btRigidBody;
class btCollisionShape;
class btTriangleIndexVertexArray;
class btDynamicsWorld;
class btMotionState;

namespace jar
{
    class PhysicsWorld;
}

/** \todo Bullet prefers fewer, bigger triangle shapes. I should wield them together, possibly removing duplicates. I could do this by creating a huge array for all surfaces' vertices, adding the offsets to the indices when creating the bullet indices.
**/
struct SimpleLevel
{
    const bool LoadFromFile(const std::string& filename, std::string& out_error);

    const bool UploadToGPU(std::string& out_error);
    const bool DeleteFromGPU(std::string& out_error);
    const bool Render(std::string& out_error);

    const bool AddToPhysWorld(jar::PhysicsWorld& world, std::string& out_error);
    /** \note If removal of one Surface fails, the remaining ones won't be removed, either!
    **/
    const bool DeleteFromPhysWorld(jar::PhysicsWorld& world, std::string& out_error);

    static void BindToLua(lua_State* L);

    SimpleLevel();
    ~SimpleLevel();

    struct Entity
    {
        std::string classname;
        typedef std::map<std::string, std::string> PropertyMap;
        PropertyMap properties;
    };

    typedef std::list<Entity> EntityList;

    const EntityList& GetEntities() const { return mEntities; }
    
private:
    static const unsigned int VERSION = 2;
    static const unsigned int MAX_PATH_LENGTH = 64;
    static const char IDENT[8];

    struct Header
    {
        char ident[8]; //"MRWLVL1\0"
        unsigned int version;
        unsigned int numSurfaces;
    };

    struct Surface
    {
        Surface() :
            vertices(NULL),
            triangles(NULL)
        {}

        ~Surface()
        {
            delete[] vertices;
            delete[] triangles;
        }

        const bool CreatePhysBody(std::string& out_error);
        const bool DeletePhysBody(std::string& out_error);
        const bool AddToPhysWorld(btDynamicsWorld& world, std::string& out_error);
        const bool DeleteFromPhysWorld(btDynamicsWorld& world, std::string& out_error);
        
        struct Vertex
        {
            float coordinates[3];
            float uv[2];
        };

        struct Triangle
        {
            unsigned short indices[3];
        };

        enum SurfaceFlags
        {
            sfSolid = 1 << 0, // Whether this surface is solid, as far as the Physics Engine is concerned
            sfInvisible = 1 << 1, // Whether not to render this surface (only makes sense when solid)
        };

        //char texture[MAX_PATH_LENGTH]; // NULL-Terminated string containing texture name
        unsigned char color[3];
        int flags; // Combination of Surface Flags
        unsigned short numVertices;
        unsigned short numTriangles;
        Vertex* vertices; // array with numVertices elements
        Triangle* triangles; // array with numTriangles elements

        // Surface-related things only needed at runtime and thus not part of the file format:
        struct RuntimeData
        {
            RuntimeData();
            ~RuntimeData();
            unsigned int VBOIndices[2];
            int* physTriangles; // triangle indices as integers, for bullet
            btTriangleIndexVertexArray* physMesh;
            btCollisionShape* physCollisionShape;
            btRigidBody* physBody;
            btMotionState* physMotionState;

            const bool CreatePhysBody(std::string& out_error, Surface& surface);
            const bool DeletePhysBody(std::string& out_error);
        };
        RuntimeData runtimeData;
    };

    Header mHeader;
    Surface* mSurfaces; //array with header.numSurfaces elements
    EntityList mEntities; //the rest of the file are entities in plain text - { "key" "value" "key" "value" ... } { "key" "value" ... } ...
   
    bool mUploaded;
};

#endif
