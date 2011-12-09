#ifndef JAR_CORE_API_HPP
#define JAR_CORE_API_HPP

#include "jar/OS.hpp"

#ifdef JAR_CORE_LIBRARY_BUILD
#define JARCOREAPI JAR_DLLEXPORT
#else
#define JARCOREAPI JAR_DLLIMPORT
#endif

#endif
