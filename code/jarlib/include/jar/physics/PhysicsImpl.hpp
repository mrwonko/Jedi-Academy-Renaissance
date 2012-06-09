#ifndef JAR_PHYSICS_AUDIOIMPL_HPP
#define JAR_PHYSICS_AUDIOIMPL_HPP

#include "jar/core/Component.hpp"

namespace jar {

class PhysicsImpl : public Component
{
    public:
        PhysicsImpl();
        virtual ~PhysicsImpl();

        virtual const bool Init();

        virtual const bool Deinit();

    private:
};

} // namespace jar

#endif
