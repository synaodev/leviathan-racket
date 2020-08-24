# Leviathan Racket
This is the current official repository for Leviathan Racket.
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
  - CMake version must be at least 3.15.
  - OpenGL driver must support at least a 3.3 core profile.
  - Compiler must support at least C++17 and C11.
  - To manage dependencies outside of Linux, I recommend [vcpkg](https://github.com/microsoft/vcpkg).
  - Install dependencies on vcpkg like this: `<vcpkg-cli> install openal-soft angelscript glm entt sdl2 tmxlite nlohmann-json stb`.
  - Then, when running cmake, pass the toolchain file: `cmake <build-root> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake`.
- Linux:
  - Depending on your GPU, you may need to install non-free firmware to use OpenGL.
  - For OpenAL, GLM, SDL2, and Nlohmann-Json, you should use your package manager:
    - Apt: `apt-get install libgl1-mesa-dev mesa-utils libopenal-dev libglm-dev libsdl2-dev nlohmann-json-dev`
    - Pacman: `pacman -S mesa openal glm sdl2 nlohmann-json`
    - Yum: `yum install mesa-libGL-devel openal-soft-devel glm-devel SDL2-devel json-devel`
  - For Angelscript and EnTT, build and install from source using cmake.
  - Tmxlite should also be built from source using cmake, but I recommend adding this argument when running cmake: `-DTMXLITE_STATIC_LIB:BOOL=TRUE`.
  - For STB, clone the [repository](https://github.com/nothings/stb) and copy `stb_image.h` to `/usr/local/include`.
- MacOS:
  - Vcpkg is absolutely required.
  - Install Xcode and Xcode Command-Line Tools in order to use OpenGL.Framework (yes, seriously).
  - Supports only AppleClang for now. GCC support is coming soon, and it might require the use of [homebrew](https://brew.sh/), but we'll see.
  - When installing dependencies with vcpkg, leave out openal-soft. It often exhibits strange behavior, and it also usually gets ignored in favor of OpenAL.Framework.
- Windows:
  - Vcpkg is absolutely required.
  - Supports MSVC, Clang, and MinGW (Posix threading model only). Cygwin environment is not supported. You can also use MinGW & vcpkg on Linux for cross-compiling.
  - Additionally, for MinGW, you need to build vcpkg dependencies using a MinGW triplet.
  - To do this, open `<vcpkg-root>/scripts/toolchains/mingw.cmake` and add the following lines:
    - `set(CMAKE_C_COMPILER "<mingw-gcc-posix>")`
    - `set(CMAKE_CXX_COMPILER "<mingw-g++-posix>")`
    - `set(CMAKE_RC_COMPILER "<mingw-windres>")`
    - `set(CMAKE_RC_COMPILER "<mingw-windres>")`
    - `set(CMAKE_FIND_ROOT_PATH "<paths-to-additional-dependencies>")`
    - `set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)`
    - `set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)`
    - `set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)`
    - `set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)`
  - The "additional dependencies" variable needs to be set to directories containing these libraries:
    - libwinpthread-1.dll
    - libgcc_s_seh-1.dll
    - libstdc++-6.dll
  - If vcpkg fails to build SDL2 using MinGW and cites the `-mwindows` flag as a problem for pkgconfig, open SDL2's portfile and comment out the call to `vcpkg_fixup_pkgconfig`.
  - When running cmake, you then need to specifiy your target triplet and your compiler locations like this: `cmake <build-root> -G "MinGW Makefiles" -DCMAKE_C_COMPILER=<mingw-gcc-posix> -DCMAKE_CXX_COMPILER=<mingw-g++-posix> -DCMAKE_RC_COMPILER=<mingw-windres> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-<linking-type>`
  - If cross-compiling, leave out the `-G "MinGW Makefiles"` argument.
## Python Scripts
The dependencies required to run the python scripts are in `scripts/requirements.txt`.
- `scripts/fix_fontatlas.py` fixes transparency problems in BFMC's auto-generated font atlases.
- `scripts/make_config.py` generates a default 'boot.cfg' file in a given directory.
- `scripts/make_palette.py` generates palettes and indexed textures from input textures.
