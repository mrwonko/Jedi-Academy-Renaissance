#include "jar/core/Time.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>

namespace jar
{

static sf::Clock g_clock;

TimeType GetTime()
{
	return TimeType(g_clock.getElapsedTime().asMilliseconds());
}

void Sleep(TimeType duration)
{
	sf::sleep(sf::milliseconds(duration));
}

} //namespace jar
