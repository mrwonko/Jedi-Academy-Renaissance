#include "jar/Physics.hpp"
#include "jar/physics/PhysicsImpl.hpp"
#include <cstddef>

namespace jar {

Physics* Physics::mSingleton = NULL;

Physics::Physics() :
    mImpl(new PhysicsImpl)
{
    assert(mImpl);
    assert(!mSingleton);
    mSingleton = this;
}

Physics::~Physics()
{
    delete mImpl;
    assert(mSingleton);
    mSingleton = NULL;
}

} // namespace jar
