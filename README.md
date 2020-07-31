# Leviathan Racket
This is the current official repository for Leviathan Racket.
It's a computer game inspired by my love for Cave Story, Linguistics, and Geopolitics.
I hope to release in Winter 2020.
## Python Scripts
The dependencies required to run the python scripts are in 'requirements.txt'.
- 'fix_fontatlas.py' fixes transparency problems in BFMC's auto-generated font atlases.
- 'make_config.py' generates a default 'boot.cfg' file in a given directory. 
- 'make_palette.py' generates palettes and indexed textures from input textures.
## Required Dependencies
- [OpenGL v3.3](https://www.opengl.org)
- [OpenAL-Soft](https://github.com/kcat/openal-soft)
- [Angelscript](https://github.com/codecat/angelscript-mirror)
- [GLM](https://github.com/g-truc/glm)
- [EnTT](https://github.com/skypjack/entt)
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [Tmxlite](https://github.com/fallahn/tmxlite)
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [stb](https://github.com/nothings/stb)
## Included Dependencies
- [GLAD OpenGL Loader Generator](https://glad.dav1d.de)
- [Pxtone Source](https://pxtone.org/developer)
## Building From Source
- General:
  - Currently, this builds and runs on desktop Windows, MacOS, and Linux Desktop.
  - Must have compiler with C++17 support and at least CMake v3.15.
  - MinGW is currently not supported.
- Windows:
  - To manage dependencies, I recommend [vcpkg](https://github.com/microsoft/vcpkg), but it isn't required.
  - In vcpkg, run `vcpkg.exe install openal-soft angelscript glm entt sdl2 tmxlite nlohmann-json stb`.
- Linux:
  - OpenGL:
    - Debian/Ubuntu: `apt-get install libgl1-mesa-dev mesa-utils`
  - OpenAL-Soft:
    - Debian/Ubuntu: `apt-get install libopenal-dev`
  - Angelscript:
    - Build from [mirror](https://github.com/codecat/angelscript-mirror) and install using cmake.
  - GLM:
    - Debian/Ubuntu: `apt-get install libglm-dev`
  - EnTT:
    - Build from [source](https://github.com/skypjack/entt) and install using cmake.
  - SDL2:
    - Debian/Ubuntu: `apt-get install libsdl2-dev`
  - Tmxlite:
    - Build from [source](https://github.com/fallahn/tmxlite) and install using cmake.
  - JSON for Modern C++:
    - Debian/Ubuntu: `apt-get install nlohmann-json-dev`
  - stb:
    - Clone from [repository](https://github.com/nothings/stb) and run `cp stb*.h /usr/local/include`.
