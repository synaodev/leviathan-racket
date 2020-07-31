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
- [stb](https://github.com/nothings/stb)
## Included Dependencies
- [GLAD OpenGL Loader Generator](https://glad.dav1d.de)
- [Pxtone Source](https://pxtone.org/developer)
## Building From Source
- General:
  - Must have compiler with C++17 support and at least CMake v3.15.
  - OpenGL v3.3 is the lowest version supported.
  - To manage dependencies, I recommend [vcpkg](https://github.com/microsoft/vcpkg). It's only required for Windows, but you can use it for MacOS and Linux as well.
  - Run `vcpkg install openal-soft angelscript glm entt sdl2 tmxlite nlohmann-json stb`.
  - When running cmake, pass the toolchain file: `cmake <build-directory> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake`.
- Windows:
  - Limited to MSVC for now, unfortunately.
  - Vcpkg is also required, so follow those instructions.
- MacOS:
  - Install Xcode and Xcode Command-Line Tools in order to use OpenGL headers (yes, really).
  - You can install several required packages using homebrew (Angelscript, GLM, EnTT, Nlohmann JSON), but openal-soft is always ignored in favor of OpenAL.Framework, which is also true of sdl2 and SDL2.Framework. Additionally, SDL2.Framework can cause linking errors so it's probably best to stick with vcpkg for now.
- Linux:
  - Debian/Ubuntu:
    - Run `apt-get install libgl1-mesa-dev mesa-utils libopenal-dev libglm-dev libsdl2-dev nlohmann-json-dev`.
    - For [Angelscript](https://github.com/codecat/angelscript-mirror), [EnTT](https://github.com/skypjack/entt), and [Tmxlite](https://github.com/fallahn/tmxlite), build and install from source using cmake.
    - For [stb](https://github.com/nothings/stb), clone the repository and run `cp stb*.h /usr/local/include`.
## Python Scripts
The dependencies required to run the python scripts are in 'requirements.txt'.
- 'fix_fontatlas.py' fixes transparency problems in BFMC's auto-generated font atlases.
- 'make_config.py' generates a default 'boot.cfg' file in a given directory. 
- 'make_palette.py' generates palettes and indexed textures from input textures.
