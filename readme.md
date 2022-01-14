# Notice
From November 2019 to January 2022, this was the official repository for Apostellein (previously known as
Leviathan Racket). The current official repository is located [here](https://github.com/synaodev/apostellein).
I decided on following through with this major rewrite for several reasons, the most important of which being that
my personal standards for code have changed rather dramatically. I frequently felt that I was spending considerable
time making the code more readable and easier to maintain, rather than making content or debugging. Additionally,
I came to dislike the tooling that I had chosen earlier in development, like using angelscript for scripting, using
fmt::print for logging instead of spdlog, using python to write convoluted utility scripts, etc.

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
  - Supports MSVC and Clang. Neither MinGW nor Cygwin are supported.
  - Both x86 and AMD64 builds should work, but x86 is tested infrequently.
