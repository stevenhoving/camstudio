# CamStudio

[![Build status](https://ci.appveyor.com/api/projects/status/fj6dyhgs69l6s2jg?svg=true)](https://ci.appveyor.com/project/stevenhoving/camstudio)


## The fork
This is a personal fork of the camstudio source code found in the mecurial repository on sourceforge.net.
I have forked this project because:
* The installers that are availible on the web are not kosher.
* No h264/mkv support out of the box.
* Flash based products are insecure, support must be removed.

## Goals
Besides creating a clean installer for camstudio I have found myself a couple of additional goals.

* Fix memory leaks - done.
* Remove unmaintainable features:
  * swf support - removed.
  * xnote support - removed.
* Upgrade/Remove project dependencies:
  * CxImage - removed.
  * zlib - done.
  * minilzo - done.
  * libconfig++ - removed.
  * libpng - removed.
* Replace static visual studio project files with CMake - done.
* Add support for mkv - done.
* Replace libcondig with alternative - done.
* Improve settings screen - done.
* Create release - in progress.
* Introduce unit test - in progress.
* Replace UI framework with Qt.

## Settings dialogs
![video settings dialog](https://raw.githubusercontent.com/stevenhoving/camstudio/develop/assets/github/video-settings-dialog.png)
![cursor settings dialog](https://raw.githubusercontent.com/stevenhoving/camstudio/develop/assets/github/cursor-settings-dialog.png)
![shortcuts settings dialog](https://raw.githubusercontent.com/stevenhoving/camstudio/develop/assets/github/shortcuts-settings-dialog.png)
![application settings dialog](https://raw.githubusercontent.com/stevenhoving/camstudio/develop/assets/github/application-settings-dialog.png)

## Removed features
* swf support.
* xnote support.

## How to clone/build
1. `git clone https://github.com/stevenhoving/camstudio.git --recursive`
2. `bootstrap.cmd`
3. Now open the project in the `vs_build` directory and build it.

## The source forge project
https://sourceforge.net/p/camstudio

## Dependencies
State | Name | Project
----- | -----|--------
Official| minilzo | http://www.oberhumer.com/opensource/lzo
Official| zlib    | https://github.com/madler/zlib
Official| ffmpeg  | https://www.ffmpeg.org
Official| cpptoml | https://github.com/skystrife/cpptoml
Official| fmtlib  | https://github.com/fmtlib/fmt
Official| Aeon Cmake | https://github.com/aeon-engine/libaeon-cmake
Official| Google Test  | https://github.com/google/googletest
Official| Google Benchmark | https://github.com/google/benchmark

## Credits
A big thanks goes out to the original CamStudio developers. And to bestlily who had a initial port of CamStudio (https://github.com/bestlily/Camstudio), thanks.
