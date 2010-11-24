#ifndef JAR_INPUT_API_HPP
#define JAR_INPUT_API_HPP

#ifdef JAR_INPUT_LIBRARY_BUILD
#define JARINPUTAPI __declspec(dllexport)
#else
#define JARINPUTAPI __declspec(dllimport)
#endif

#endif
