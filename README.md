# Xash3D FWGS Engine [![Build Status](https://travis-ci.org/FWGS/xash3d.svg)](https://travis-ci.org/FWGS/xash3d) [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/FWGS/xash3d?svg=true)](https://ci.appveyor.com/project/a1batross/xash3d)

Latest version of the engine is available at:
https://github.com/FWGS/xash3d

Orignal project: [Xash3D on ModDB](http://www.moddb.com/engines/xash3d-engine)

Xash3D Engine is a custom Gold Source engine rewritten from scratch. Xash3D
is compatible with many of the Gold Source games and mods and should be
able to run almost any existing singleplayer Half-Life mod without a hitch.
The multiplayer part is not yet completed, multiplayer mods should work just
fine, but bear in mind that some features may not work at all or work not
exactly the way they do in Gold Source Engine.

Xash3D FWGS is a fork of Xash3D Engine, which aims on crossplatform and compability 
with original Xash3D and Gold Source.

- [Building and running Xash3D FWGS](https://github.com/FWGS/xash3d/wiki/Building-and-running)
- [Installing and setting up the server on Debian/Ubuntu Linux](https://github.com/FWGS/xash3d/wiki/How-to-set-up-a-Xash3D-Dedicated-Server-on-Debian-Ubuntu-Linux)

# License

The library is licensed under GPLv3 license, see [COPYING](https://github.com/FWGS/xash3d/blob/master/COPYING) for details.
CMakeLists.txt files are licensed under MIT license, you will find it's text
in every CMakeLists.txt header.

# Building for the Classic Mini Series

```
cd engine
git clone https://github.com/FWGS/nanogl
cd ..
```
Then, edit `mainui/CMakeLists.txt`, and switch the MAINUI_USE_CUSTOM_FONT_RENDER to OFF, and the MAINUI_USE_STB to ON.
```
mkdir build
cd build
cmake -DXASH_NANOGL=yes -DXASH_GLES=yes ..
make
```
Done!
