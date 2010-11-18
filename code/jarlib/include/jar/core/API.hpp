#ifndef JAR_CORE_API_HPP
#define JAR_CORE_API_HPP

#ifdef JAR_CORE_LIBRARY_BUILD
#define JARCOREAPI __declspec(dllexport)
#else
#define JARCOREAPI __declspec(dllimport)
#endif

#endif
