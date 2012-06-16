#include "jar/physics/PhysicsWorld.hpp"
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>

namespace jar
{

    PhysicsWorld::PhysicsWorld() :
    mWorld(NULL)
{
    btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();
    mWorld = new btDiscreteDynamicsWorld(new btCollisionDispatcher(collisionConfig), new btDbvtBroadphase(), new btSequentialImpulseConstraintSolver(), collisionConfig);
}

PhysicsWorld::~PhysicsWorld()
{
    if(mWorld)
    {
        btBroadphaseInterface* broadphase = mWorld->getBroadphase();
        btCollisionDispatcher* dispatcher = reinterpret_cast<btCollisionDispatcher*>(mWorld->getDispatcher());
        btConstraintSolver* solver = mWorld->getConstraintSolver();
        btCollisionConfiguration* collisionConfig = dispatcher->getCollisionConfiguration();
        delete mWorld;
        mWorld = NULL;
        delete solver;
        delete broadphase;
        delete dispatcher;
        delete collisionConfig;
    }
}

}
