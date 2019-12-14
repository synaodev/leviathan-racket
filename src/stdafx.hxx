#ifndef SYNAO_PRECOMPILED_HEADER_STDAFX_HXX
#define SYNAO_PRECOMPILED_HEADER_STDAFX_HXX

#include <new>
#include <limits>
#include <vector>
#include <array>
#include <string>
#include <thread>
#include <atomic>
#include <future>
#include <memory>
#include <unordered_map>
#include <map>
#include <set>
#include <bitset>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <streambuf>
#include <locale>
#include <iosfwd>
#include <optional>
#include <functional>
#include <cassert>
#include <algorithm>

#if !defined(__APPLE__) && !defined(__GNUC__)
#include <filesystem>
#else // __APPLE__ or __GNUC__
#include <dirent.h>
#endif // __APPLE__ or __GNUC__

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <SDL2/SDL.h>

#include "./utl/json.hpp"

#endif // SYNAO_PRECOMPILED_HEADER_STDAFX_HXX