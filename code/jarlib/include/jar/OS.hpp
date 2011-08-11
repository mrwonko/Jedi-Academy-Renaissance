#ifndef JAR_OS_HPP
#define JAR_OS_HPP

#if defined(_WIN32)

  #define JAR_DLLEXPORT __declspec(dllexport)
  #define JAR_DLLIMPORT __declspec(dllimport)

#else

  #if __GNUC__ >= 4

    #define JAR_DLLEXPORT __attribute__ ((visibility ("default")))
    #define JAR_DLLIMPORT __attribute__ ((visibility ("default")))

  #else

    #define JAR_DLLEXPORT
    #define JAR_DLLIMPORT

  #endif

#endif

#endif //JAR_OS_HPP
