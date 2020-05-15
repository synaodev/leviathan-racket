# Leviathan Racket
This is the current official repository for Leviathan Racket.
It's a computer game inspired by my love for Cave Story, Linguistics, and Geopolitics.
I hope to release in Winter 2020.
## Building From Source
- Currently, Windows, MacOS, and Linux are all supported.
- Minimum required CMake version is 3.15.
- C++ compiler must have C++17 support.
- [vcpkg](https://github.com/microsoft/vcpkg) is recommended to manage dependencies (Not required).
- If you use vcpkg: 
  - After installing `txmlite`, you should run the 'fix_tmxlite.py' script to fix the default installation.
  - When running the cmake build script, set the toolchain file to `<vcpkg_root>/scripts/buildsystems/vcpkg.cmake`.
## Emscripten Port (Experimental!)
- This build process is only guaranteed to work on Linux, but the actual port itself is a work in progress.
- After cloning this repository, run `git submodule update --init --recursive`.
- This retrieves dependencies pre-built with emscripten so they can be linked without too much trouble.
- Clone the [SDK](https://github.com/emscripten-core/emsdk), activate it and set your environment variables like this: `source <emsdk_root>/emsdk_env.sh`.
- Now run the cmake build script like this: `emcmake cmake <project_root> -DEMSCRIPTEN=TRUE`.
- If everything goes as planned, can run the generate Makefile like this: `emmake make`.
- This currently generates an HTML file, a WebAssembly file, a Javascript file, and virtual filesystem data.
### Python Scripts
The dependencies required to run the python scripts are in 'requirements.txt'.
- 'fix_fontatlas.py' fixes transparency problems in BFMC's auto-generated font atlases.
- 'fix_tmxlite.py' fixes the vcpkg installation of Tmxlite so it can be properly used in CMake.
- 'make_palette.py' generates palettes and indexed textures from input textures.
### Required Dependencies
- OpenGL:
  - For Desktop build, version must be OpenGL 3.3 or above.
  - For Emscripten port, version must be WebGL 2.0 (OpenGLES 3.0) or above.
- OpenAL:
  - For proprietary OpenAL, version must be 1.1.
  - For open source [OpenAL-Soft](https://github.com/kcat/openal-soft), version must be 1.19.0 or above.
  - If using vcpkg for dependencies, install `openal-soft`.
- [Angelscript v2.34.0](https://github.com/codecat/angelscript-mirror)
- [GLM v0.9.9.8](https://github.com/g-truc/glm)
- [EnTT v3.3.2](https://github.com/skypjack/entt)
- [SDL v2.0.12](https://www.libsdl.org/download-2.0.php)
- [Tmxlite v1.1.0](https://github.com/fallahn/tmxlite)
### Included Dependencies
- [Json For Modern C++ v3.7.3](https://github.com/nlohmann/json)
- [Image Loading Header Library](https://github.com/nothings/stb)
- [Glad OpenGL Loader Generator](https://glad.dav1d.de)
- [Pxtone Source](https://pxtone.org/developer)
