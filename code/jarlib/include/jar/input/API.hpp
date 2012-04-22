#ifndef JAR_INPUT_API_HPP
#define JAR_INPUT_API_HPP

#include "jar/OS.hpp"

#ifdef JAR_INPUT_LIBRARY_BUILD
#define JARINPUTAPI JAR_DLLEXPORT
#else
#define JARINPUTAPI JAR_DLLIMPORT
#endif

#endif
