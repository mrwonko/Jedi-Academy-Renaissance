#ifndef JAR_PHYSICS_HPP
#define JAR_PHYSICS_HPP

#include "jar/physics/API.hpp"
#include <cassert>

namespace jar {

class PhysicsImpl;

class JARPHYSICSAPI Physics
{
    public:
        Physics();
        virtual ~Physics();

        static Physics& GetSingleton() { assert(mSingleton); return *mSingleton; }

    private:
        PhysicsImpl* mImpl;
        static Physics* mSingleton;
};

} // namespace jar

#endif // JAR_INPUT_INPUT_HPP
