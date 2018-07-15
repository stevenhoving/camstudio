# CamStudio

![AppVeyor](https://ci.appveyor.com/api/projects/status/github/stevenhoving/camstudio?branch=develop&svg=true)


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
  * swf support - done.
  * xnote support - done.
* Upgrade/Remove project dependencies:
  * CxImage - canceled.
  * zlib - done.
  * minilzo - done.
  * libconfig++ - removed.
  * libpng - done.
* Replace static visual studio project files with CMake - done.
* Create release.
* Introduce unit test - in progress.
* Add support for mkv - done.
* Replace libcondig with alternative - done.
* Replace UI framework with Qt.
* Improve settings screen.

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
Official| zlib | https://github.com/madler/zlib
Fork| CxImage | https://github.com/movableink/cximage
Official| ffmpeg | https://www.ffmpeg.org
Official| cpptoml | https://github.com/skystrife/cpptoml
Official| fmtlib | https://github.com/fmtlib/fmt

## Credits
A big thanks goes out to the original CamStudio developers. And to bestlily who had a initial port of CamStudio (https://github.com/bestlily/Camstudio), thanks.
