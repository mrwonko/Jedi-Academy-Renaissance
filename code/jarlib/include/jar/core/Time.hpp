#ifndef JAR_CORE_TIME_HPP
#define JAR_CORE_TIME_HPP

#include "jar/core/API.hpp"

namespace jar
{

/// Conversion from seconds to TimeType
const float TimeTypesPerSecond = 1000;
/// Conversion from TimeType to seconds
const float SecondsPerTimeType = 1.0f / TimeTypesPerSecond;

/// The type of time variables
typedef long int TimeType; //when you change this, check GetTime() for validity!

JARCOREAPI TimeType GetTime();

} //namespace jar

#endif //JAR_CORE_TIME_HPP
