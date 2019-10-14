rd /s /q vs_build
mkdir vs_build
cd vs_build
../cmake/bin/cmake.exe --version
cmake/bin/cmake.exe ../ -G "Visual Studio 16 2019" -A x64 -T host=x64
cd ..