# Raytracer
### Raytracer developed for CS 488 Introduction to Computer Graphics (Winter 2020)
This was going to be my submission for the final project, but due to COVID-19 shutdown unfortunately the project was cancelled.

---

## Bundled dependencies
* Lua
    * http://www.lua.org/
* Premake4
    * https://github.com/premake/premake-4.x/wiki
    * http://premake.github.io/download.html
* GLM
    * http://glm.g-truc.net/0.9.7/index.html
* LodePNG
    * https://lodev.org/lodepng/


---

## Build instructions
We use **premake4** as our cross-platform build system. First you will need to build all
the static libraries that the ray tracer depend on. To build the libraries, open up a
terminal, and **cd** to the top level of the project directory (`raytracer`) and then run the
following:

    /raytracer$ premake4 gmake
    /raytracer$ make

Next we can build the actual raytracing program:

    /raytracer$ cd A5-Project/
    /raytracer/A5-Project$ premake4 gmake
    /raytracer/A5-Project$ make
