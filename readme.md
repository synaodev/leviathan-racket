# Leviathan Racket
This is the current official repository for Leviathan Racket. I work on this game in my spare time, so there is no release date currently.
## Required Dependencies
- [fmt](https://github.com/fmtlib/fmt)
- [openal](https://github.com/kcat/openal-soft)
- [angelscript](https://github.com/codecat/angelscript-mirror)
- [glm](https://github.com/g-truc/glm)
- [entt](https://github.com/skypjack/entt)
- [sdl2](https://www.libsdl.org/download-2.0.php)
- [nlohmann-json](https://github.com/nlohmann/json)
## Included Dependencies
- [ghc](https://github.com/gulrak/filesystem)
- [glad](https://glad.dav1d.de)
- [imgui](https://github.com/ocornut/imgui)
- [stb](https://github.com/nothings/stb)
- [pxtone](https://pxtone.org/developer)
- [tmxlite](https://github.com/fallahn/tmxlite)
- [pugixml](https://github.com/zeux/pugixml)
- [miniz](https://github.com/richgel999/miniz)
## Building From Source
- General:
  - CMake version must be at least 3.13.
  - OpenGL driver must support at least a 3.3 core profile.
  - Compiler must support at least C++17 and C11.
  - To manage dependencies outside of Linux, I recommend [vcpkg](https://github.com/microsoft/vcpkg).
  - Install dependencies on vcpkg like this: `<vcpkg-cli> install openal-soft angelscript glm entt sdl2 fmt nlohmann-json`.
  - Then, when running cmake, pass the toolchain file: `cmake <build-root> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake`.
- Linux:
  - Depending on your GPU, you may need to install non-free firmware.
  - For OpenGL, OpenAL, glm, SDL2, fmt, and nlohmann-json, you should use your package manager:
    - Apt: `apt-get install libgl1-mesa-dev mesa-utils libopenal-dev libglm-dev libsdl2-dev libfmt-dev nlohmann-json3-dev`
    - Pacman: `pacman -S mesa openal glm sdl2 fmt nlohmann-json`
    - Yum: `yum install mesa-libGL-devel openal-soft-devel glm-devel SDL2-devel fmt-devel json-devel`
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
    - `libwinpthread-1.dll`
    - `libgcc_s_seh-1.dll`
    - `libstdc++-6.dll`
  - If vcpkg fails to build SDL2 using MinGW and cites the `-mwindows` flag as a problem for pkgconfig, open SDL2's portfile and comment out the call to `vcpkg_fixup_pkgconfig`.
  - When running cmake, you then need to specifiy your target triplet and your compiler locations like this: `cmake <build-root> -G "MinGW Makefiles" -DCMAKE_C_COMPILER=<mingw-gcc-posix> -DCMAKE_CXX_COMPILER=<mingw-g++-posix> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic`
  - If cross-compiling, leave out the `-G "MinGW Makefiles"` argument.
## Python 3 Scripts
The dependencies required to run the python scripts are in `requirements.txt`.
- `make-config.py` generates a default configuration file called `boot.cfg`.
- `make-tilekey.py` opens an editor that allows for modification of tileset attributes. Not complete.
- `make-palette.py` generates a palette and index texture from a given texture atlas.
- `make-symlink.py` creates local links to SDL_PrefPath() directories. Helpful for debugging.
## Thank You
- [Daisuke Amaya](https://en.wikipedia.org/wiki/Daisuke_Amaya):
  The developer of Cave Story. He also currently develops Pxtone Collage, which I am using to compose the soundtrack. If not for his work, this project would not exist.
- [Christopher Hebert](https://github.com/chebert):
  The creator of an extraordinary video series called [Reconstructing Cave Story](https://youtube.com/playlist?list=PL006xsVEsbKjSKBmLu1clo85yLrwjY67X). The tilemap collision system is heavily based on his tilemap collision system.
- [SFML Team](https://github.com/sfml):
  The UTF8-UTF32 conversion functions and the *lCheck() debug macros all come from the [SFML library](https://github.com/sfml/sfml).
- [Jakob Progsch](https://github.com/progschj):
  The thread pool implementation is based on [his repository](https://github.com/progschj/ThreadPool).
- [Mikola Lysenko](https://github.com/mikolalysenko):
  The tilemap raycast algorithm is based on [his repository](https://github.com/mikolalysenko/voxel-raycast).
- [Caitlin Shaw](http://nxengine.sourceforge.net/):
  The actor routine tables are generated using C++ preprocessor macros. This idea comes from the rather unorthodox InitList system in [NxEngine](https://github.com/nxengine/nxengine-evo/), which is used to initialize AI functions at startup.
