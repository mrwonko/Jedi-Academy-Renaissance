#ifndef JAR_CORE_COMPONENT_HPP
#define JAR_CORE_COMPONENT_HPP

#include "jar/core/API.hpp"
#include "jar/core/Time.hpp"

namespace jar {

/** \brief All the other components of jarlib, e.g. jar-input, inherit from this class, which automatically registers them with the Core once they are created. **/
class JARCOREAPI Component
{
    public:
        Component();
        virtual ~Component();

        virtual const bool Init() = 0;
        /** \brief Deinitializes the component, if it has been initialized yet. Does nothing otherwise. **/
        virtual const bool Deinit() = 0;
        virtual void Update(const TimeType deltaT);

    protected:
    private:
};

} // namespace jar

#endif // JAR_CORE_COMPONENT_HPP
