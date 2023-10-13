# Introduction

Client is programmed using C++ and built using CMake. Uses SDL2 for graphics, input, TTF, and networking.

## Build

If you have SDL2 system libraries installed:

``` text
cmake -S . -B build
cmake --build build
```

If you want to use a local SDL2 version:

Copy SDL2 package contents to ./vendored/sdl

``` text
cmake -S . -B build -DTypeTank_VENDORED=ON
cmake --build build
```
