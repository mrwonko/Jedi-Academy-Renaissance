#ifndef JAR_AUDIO_API_HPP
#define JAR_AUDIO_API_HPP

#include "jar/OS.hpp"

#ifdef JAR_AUDIO_LIBRARY_BUILD
#define JARAUDIOAPI JAR_DLLEXPORT
#else
#define JARAUDIOAPI JAR_DLLIMPORT
#endif

#endif
