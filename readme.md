# Leviathan Racket
This is the current official repository for Leviathan Racket.
I hope to release in Winter 2021.
## Required Dependencies
- [fmt](https://github.com/fmtlib/fmt)
- [openal](https://github.com/kcat/openal-soft)
- [angelscript](https://github.com/codecat/angelscript-mirror)
- [glm](https://github.com/g-truc/glm)
- [entt](https://github.com/skypjack/entt)
- [sdl2](https://www.libsdl.org/download-2.0.php)
## Included Dependencies
- [ghc](https://github.com/gulrak/filesystem)
- [glad](https://glad.dav1d.de)
- [imgui](https://github.com/ocornut/imgui)
- [json](https://github.com/nlohmann/json)
- [stb](https://github.com/nothings/stb)
- [pxtone](https://pxtone.org/developer)
- [tmxlite](https://github.com/fallahn/tmxlite)
- [toml++](https://github.com/marzer/tomlplusplus)
- [pugixml](https://github.com/zeux/pugixml)
- [miniz](https://github.com/richgel999/miniz)
## Building From Source
- General:
  - CMake version must be at least 3.13.
  - OpenGL driver must support at least a 3.3 core profile.
  - Compiler must support at least C++17 and C11.
  - To manage dependencies outside of Linux, I recommend [vcpkg](https://github.com/microsoft/vcpkg).
  - Install dependencies on vcpkg like this: `<vcpkg-cli> install openal-soft angelscript glm entt sdl2 fmt`.
  - Then, when running cmake, pass the toolchain file: `cmake <build-root> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake`.
- Linux:
  - Depending on your GPU, you may need to install non-free firmware.
  - For OpenGL, OpenAL, GLM, SDL2, and fmt, you should use your package manager:
    - Apt: `apt-get install libgl1-mesa-dev mesa-utils libopenal-dev libglm-dev libsdl2-dev libfmt-dev`
    - Pacman: `pacman -S mesa openal glm sdl2 fmt`
    - Yum: `yum install mesa-libGL-devel openal-soft-devel glm-devel SDL2-devel fmt-devel`
  - For Angelscript and EnTT build and install from source using cmake.
- MacOS:
  - Vcpkg is absolutely required.
  - Install Xcode and Xcode Command-Line Tools in order to use OpenGL.Framework (yes, seriously).
  - Supports only AppleClang.
  - When installing dependencies with vcpkg, leave out openal-soft. It usually gets ignored in favor of OpenAL.Framework.
- Windows:
  - Vcpkg is absolutely required.
  - Supports MSVC, Clang, and MinGW (Posix threading model only). Cygwin environment is not supported. You can also use MinGW & vcpkg on Linux for cross-compiling.
  - Additionally, for MinGW, you need to build vcpkg dependencies using a MinGW triplet. The most consistently functional triplet is "x64-mingw-dynamic", so you might want to go with that one.
  - To do this, open `<vcpkg-root>/scripts/toolchains/mingw.cmake` and add the following lines:
    - `set(CMAKE_C_COMPILER "<mingw-gcc-posix>")`
    - `set(CMAKE_CXX_COMPILER "<mingw-g++-posix>")`
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
  - When running cmake, you then need to specifiy your target triplet and your compiler locations like this: `cmake <build-root> -G "MinGW Makefiles" -DCMAKE_C_COMPILER=<mingw-gcc-posix> -DCMAKE_CXX_COMPILER=<mingw-g++-posix> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic`
  - If cross-compiling, leave out the `-G "MinGW Makefiles"` argument.
## Scripts
The dependencies required to run the python scripts are in `requirements.txt`. The bash scripts only run on Linux, unfortunately.
- `make-config.py` generates a default configuration file called `boot.cfg`.
- `make-tilekey.py` opens an editor that allows for modification of tileset attributes.
- `make-palette.py` generates palettes and index textures from a given texture atlas.
- `make-symlink.py` creates symlinks for SDL_PrefPath() directories in the "data" folder.
- `windows.sh` cross-compiles and packs the game for Windows using vcpkg, cmake, mingw, and 7zip.
## Thank You
- SFML Team:
  glCheck, alCheck, and the UTF32 to UTF8 conversion functions come from them.
- Daisuke Amaya:
  He created Cave Story, which inspired this game, and develops Pxtone, which I used to compose the music.
