#ifndef JAR_GRAPHICS_API_HPP
#define JAR_GRAPHICS_API_HPP

#include "jar/OS.hpp"

#ifdef JAR_GRAPHICS_LIBRARY_BUILD
#define JARGRAPHICSAPI JAR_DLLEXPORT
#else
#define JARGRAPHICSAPI JAR_DLLIMPORT
#endif

#endif
