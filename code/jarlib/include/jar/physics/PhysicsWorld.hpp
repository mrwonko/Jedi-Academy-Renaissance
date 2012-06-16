#ifndef JAR_PHYSICS_PHYSICS_WORLD_HPP
#define JAR_PHYSICS_PHYSICS_WORLD_HPP

#include "jar/physics/API.hpp"
#include <cassert>

class btDynamicsWorld;

namespace jar
{

/** \brief Simplified version of the btDiscreteDynamicsworld
**/
class JARPHYSICSAPI PhysicsWorld
{
public:
    PhysicsWorld();
    ~PhysicsWorld();

    btDynamicsWorld& GetBulletWorld() { assert(mWorld); return *mWorld; }

private:
    btDynamicsWorld* mWorld;
};

}

#endif
