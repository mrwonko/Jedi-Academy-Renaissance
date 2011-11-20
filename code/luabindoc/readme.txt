I don't have a full implementation of the preprocessor symbols (I probably don't even have a complete implementation of C++...), these are the preprocessor commands I use:

* #include <file> and #include "file" mostly work as expected, except I don't distinguish between them.
* Each file is only included once, as determined by its name. (Warning: This can lead to problems with files with the same name.)
* In lieu of #if #else #endif constructs and #define I use a simplified syntax:
  * All luabind preprocessor commands are single-line comments starting with //luabindoc
  * "//luabindoc ignore begin" and "//luabindoc ignore end" mark a block to be ignored
  * "//luabindoc ignore all" causes the rest of the file to be ignored