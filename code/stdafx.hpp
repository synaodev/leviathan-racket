#ifndef SYNAO_PRECOMPILED_HEADER_HPP
#define SYNAO_PRECOMPILED_HEADER_HPP

// C standard libraries
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cassert>
#include <cstring>
#include <cstdio>

// C++ standard libraries
#include <typeinfo>
#include <bitset>
#include <functional>
#include <utility>
#include <chrono>
#include <tuple>
#include <optional>
#include <new>
#include <memory>
#include <limits>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <random>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iomanip> 
#include <streambuf>
#include <locale>
#include <atomic>
#include <thread>
#include <mutex>
#include <future>

// C++ filesystem library is weird on Apple Clang and GCC
#if !defined(__APPLE__) && !defined(__GNUC__)
#include <filesystem>
#else
#include <dirent.h>
#endif // __APPLE__ OR __GNUC__

// Other libraries
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <entt/entt.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <angelscript.h>
#include <SDL2/SDL.h>

// Etc...
#include "./types.hpp"
#include "./utl/json.hpp"

#endif // SYNAO_PRECOMPILED_HEADER_HPP