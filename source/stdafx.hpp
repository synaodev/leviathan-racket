#ifndef LEVIATHAN_INCLUDED_STDAFX_HPP
#define LEVIATHAN_INCLUDED_STDAFX_HPP

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

#ifndef TARGET_MISSING_STL_FILESYSTEM
	#include <filesystem>
#endif // TARGET_MISSING_STL_FILESYSTEM

// Other libraries
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <entt/entt.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <angelscript.h>
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>

#include "./types.hpp"

#endif // LEVIATHAN_INCLUDED_STDAFX_HPP
