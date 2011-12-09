#include "jar/core/Updatable.hpp"
#include "jar/Core.hpp"
#include "jar/core/CoreImpl.hpp"

namespace jar {

Updatable::Updatable()
{
    //ctor
    assert(Core::HasSingleton());
    Core::GetSingleton().GetImpl().RegisterUpdatable(this);
}

Updatable::~Updatable()
{
    //dtor
    if(Core::HasSingleton())
    {
        Core::GetSingleton().GetImpl().UnregisterUpdatable(this);
    }
}

void Updatable::Update(const TimeType deltaT)
{
    //some people may forget overwriting the Update() function, or don't need it but have to inherit Updatable (indirectly)
}

} // namespace jar
