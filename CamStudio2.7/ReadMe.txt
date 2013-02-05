                H O W   T O   B U I L D   C A M S T U D I O

To build CamStudio you'll need few 3rd party projects, namely libconfig and CxImage

Grab libconfig from http://www.hyperrealm.com/libconfig/
and CxImage from http://www.xdp.it/cximage.htm

Unpack both such that each reside in its own folder inside of camstudio main folder

CamStudio-T-CamLib
          +-CxImage-T-CxImage
          |         +-demo
          |         +-demo2
          |         +-doc
          |         +-jasper
          |         L-...
          +-Debug
          +-GlobalResources
          +-hook
          +-hookkey
          +-libconfig-T-debian
          |           +-doc
          |           +-examples
          |           L-lib
          +-Player
          +-PlayerPlus
          +-Producer
          +-Recorder
          L-Release


For each 3rd party project:
1. Open solution
2. Using Configuration Manager change configuration to Release
3. Press F7 and wait
