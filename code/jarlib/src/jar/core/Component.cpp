#include "jar/core/Component.hpp"
#include "jar/core/CoreImpl.hpp"
#include "jar/Core.hpp"

namespace jar {

Component::Component()
{
    //ctor
    Core::GetSingleton().GetImpl().RegisterComponent(this);
}

Component::~Component()
{
    //dtor
    if(Core::HasSingleton())
    {
        Core::GetSingleton().GetImpl().UnregisterComponent(this);
    }
}

void Component::Update(const TimeType deltaT)
{
    // implemented by inheritants
}

} // namespace jar
