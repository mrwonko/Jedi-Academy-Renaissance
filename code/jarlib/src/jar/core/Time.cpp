#include "jar/core/Time.hpp"
#include <SFML/System/Clock.hpp>

namespace jar
{

static sf::Clock g_clock;

TimeType GetTime()
{
    return TimeType(g_clock.GetElapsedTime() * TimeTypesPerSecond + 0.5f);
}

} //namespace jar
