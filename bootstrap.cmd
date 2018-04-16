rd /s /q vs_build
mkdir vs_build
cd vs_build
cmake ../ -G "Visual Studio 15 2017 Win64" -T host=x64
cd ..