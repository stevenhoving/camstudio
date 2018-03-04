# CamStudio

## The fork
This is a personal fork of the camstudio source code found in the mecurial repository on sourceforge.net.
I have forked this project because:
* The installers that are availible on the web are not kosher.
* No mkv support out of the box.
* Flash based products are insecure.

## Goals
Besides creating a clean installer for camstudio I have found myself a couple of additional goals.
* Fix memory leaks.
* Remove swf support.
* Upgrade project dependencies (CxImage, zlib, minilzo, libconfig++).
* Introduce unit test.
* Add support for mkv.
* Replace UI framework with Qt.
* Improve settings screen.

## The source forge project
https://sourceforge.net/p/camstudio

## Dependencies
State | Name | Project
----- | -----|--------
Official| libconfig | https://github.com/hyperrealm/libconfig
Official| minilzo | http://www.oberhumer.com/opensource/lzo/
Official| zlib | https://github.com/madler/zlib
Fork| CxImage | https://github.com/movableink/cximage

## Credits
A big thanks goes out to the original CamStudio developers. And to bestlily who had a initial port of CamStudio (https://github.com/bestlily/Camstudio), thanks.
