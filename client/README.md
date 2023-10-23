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
Copy SDL2_image package contents to ./vendored/sdl_image
Copy SDL2_net package contents to ./vendored/sdl_net
Copy SDL2_ttf package contents to ./vendored/sdl_ttf
Need to have freetype development library available or vendored under ./vendored/sdl_ttf/external/freetype, may just clone the one referenced in ./vendored/sdl_ttf/.gitmodule

``` text
cmake -S . -B build -DTypeTanks_VENDORED=ON
cmake --build build
```
