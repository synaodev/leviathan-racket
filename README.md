# Leviathan Racket
This is the current official repository for Leviathan Racket.
It's a computer game inspired by my love for Cave Story, Linguistics, and Geopolitics.
I hope to release in Winter 2020.
## Building From Source
- The only supported platforms are Windows, MacOS, and Linux.
- Minimum required CMake version is 3.15.
- C++ compiler must have C++17 support.
- [vcpkg](https://github.com/microsoft/vcpkg) is recommended to manage dependencies (Not required).
- If you use vcpkg to install Tmxlite, you should run the 'fix_tmxlite.py' script to fix the default installation.
### Python Scripts
The dependencies required to run the python scripts are in 'requirements.txt'.
- 'fix_fontatlas.py' fixes transparency problems in BFMC's auto-generated font atlases.
- 'fix_tmxlite.py' fixes the vcpkg installation of Tmxlite so it can be properly used in CMake.
- 'make_palette.py' generates palettes and indexed textures from input textures.
### Required Dependencies
- OpenGL 3.3 - 4.6
- [Angelscript v2.34.0](https://github.com/codecat/angelscript-mirror)
- [OpenAL-Soft v1.19.0](https://github.com/kcat/openal-soft)
- [GLM v0.9.9.5](https://github.com/g-truc/glm)
- [EnTT v3.2.2](https://github.com/skypjack/entt)
- [SDL v2.0.10](https://www.libsdl.org/download-2.0.php)
- [Tmxlite v1.1.0](https://github.com/fallahn/tmxlite)
### Included Dependencies
- [Json For Modern C++ v3.7.3](https://github.com/nlohmann/json)
- [Image Loading Header Library](https://github.com/nothings/stb)
- [Glad OpenGL Loader Generator](https://glad.dav1d.de)
- [Pxtone Source](https://pxtone.org/developer)
