# Leviathan Racket
This is the current official repository for Leviathan Racket.
It's a computer game inspired by my love for Cave Story, Shounen Manga, Linguistics, and Geopolitics.
I hope to release in Winter 2020.
## Building From Source
- C++17 compiler support is absolutely required.
- I recommend vcpkg to manage dependencies. (https://github.com/microsoft/vcpkg)
- To use a vcpkg toolchain file on the CMake CLI, add this:<br>
	"-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
- Tmxlite's vcpkg port has inconsistent CMake configuration files, so I've included a fix.
- Copy the Tmxlite files from the "/cmake" project directory to this directory:<br>
	"/path/to/vcpkg/installed/architecture/share/tmxlite"
### Required Dependencies
- OpenGL 3.3 - 4.6
- CMake v3.15 (https://cmake.org/)
- Angelscript v2.34.0 (https://www.angelcode.com/angelscript/)
- OpenAL-Soft v1.19.0 (https://github.com/kcat/openal-soft)
- GLM v0.9.9.5 (https://glm.g-truc.net/0.9.9/index.html)
- EnTT v3.2.2 (https://github.com/skypjack/entt)
- SDL v2.0.10 (https://www.libsdl.org/download-2.0.php)
- Tmxlite v1.1.0 (https://github.com/fallahn/tmxlite)
### Included Dependencies
- Json For Modern C++ v3.7.3 (https://github.com/nlohmann/json)
- Image Loading Header Library (https://github.com/nothings/stb)
- Glad OpenGL Loader Generator (https://glad.dav1d.de/)
- Pxtone Source (https://pxtone.org/developer/)
