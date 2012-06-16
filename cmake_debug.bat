mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Debug -G "Visual Studio 10" %* ..
pause
