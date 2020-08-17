# Leviathan Racket
This is the current official repository for Leviathan Racket.
It's a computer game inspired by my love for Cave Story, Linguistics, and Geopolitics.
I hope to release in Winter 2020.
## Required Dependencies
- [OpenAL-Soft](https://github.com/kcat/openal-soft)
- [Angelscript](https://github.com/codecat/angelscript-mirror)
- [GLM](https://github.com/g-truc/glm)
- [EnTT](https://github.com/skypjack/entt)
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [Tmxlite](https://github.com/fallahn/tmxlite)
- [Nlohmann JSON](https://github.com/nlohmann/json)
- [STB](https://github.com/nothings/stb)
## Included Dependencies
- [GLAD OpenGL Loader Generator](https://glad.dav1d.de)
- [Pxtone Source](https://pxtone.org/developer)
## Building From Source
- General:
  - Must have compiler with C++17 support and at least CMake v3.15.
  - OpenGL v3.3 is the lowest version supported.
  - To manage dependencies, I recommend [vcpkg](https://github.com/microsoft/vcpkg). It's required for Windows and MacOS builds, but you can use it on Linux as well.
  - `<vcpkg-CLI> install openal-soft angelscript glm entt sdl2 tmxlite nlohmann-json stb`.
  - When running cmake, pass the toolchain file: `cmake <build-directory> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake`.
- Windows:
  - Vcpkg is absolutely required.
  - Supports MSVC, Clang, and MinGW (Posix threading model only).
  - For MinGW, you need to first build vcpkg dependencies using a MinGW triplet. When running cmake, you then need to specifiy your target triplet and your compiler locations like this:
    `cmake <build-directory> \`
    `-DCMAKE_C_COMPILER=<mingw-gcc-posix> \`
    `-DCMAKE_CXX_COMPILER=<mingw-g++-posix> \`
    `-DVCPKG_TARGET_TRIPLET=x64-mingw-<linking-type>`
    `-DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake`
  - Can use MinGW & vcpkg on Linux for cross-compiling to Windows.
- MacOS:
  - Install Xcode and Xcode Command-Line Tools in order to use OpenGL headers (yes, seriously).
  - Vcpkg is absolutely required. You /can/ install several required packages using homebrew (Angelscript, GLM, EnTT, Nlohmann JSON), but openal-soft is always ignored in favor of OpenAL.Framework.
  - There are tons of other little problems (especially with SDL2 and OpenGL), but in short: It's incredibly cumbersome to setup a build environment without vcpkg.
- Linux:
  - Debian/Ubuntu:
    - Run `apt-get install libgl1-mesa-dev mesa-utils libopenal-dev libglm-dev libsdl2-dev nlohmann-json-dev`.
    - For Angelscript and EnTT, build and install from source using cmake.
	- Tmxlite should also be built from source using cmake, but I recommend adding this argument when running cmake: `-DTMXLITE_STATIC_LIB:BOOL=TRUE`
    - For STB, clone the repository and run `cp stb*.h /usr/local/include`.
## Python Scripts
The dependencies required to run the python scripts are in 'requirements.txt'.
- 'fix_fontatlas.py' fixes transparency problems in BFMC's auto-generated font atlases.
- 'make_config.py' generates a default 'boot.cfg' file in a given directory.
- 'make_palette.py' generates palettes and indexed textures from input textures.
