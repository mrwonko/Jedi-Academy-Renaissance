#ifndef JAR_CORE_UPDATABLE_HPP
#define JAR_CORE_UPDATABLE_HPP

#include "jar/core/Time.hpp"

namespace jar
{

/** \brief Automatically gets updated every frame. (If Core gets updated, that is.) **/
class JARCOREAPI Updatable
{
    public:
        Updatable();
        virtual ~Updatable();
        virtual void Update(const TimeType deltaT);
};

} // namespace jar

#endif // JAR_CORE_UPDATABLE_HPP
