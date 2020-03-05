#include <new>
#include <limits>
#include <vector>
#include <array>
#include <string>
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
#include <cassert>
#include <algorithm>

#if !defined(__APPLE__) && !defined(__GNUC__)
#include <filesystem>
#else
#include <dirent.h>
#endif // __APPLE__ OR __GNUC__

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <entt/entt.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <SDL2/SDL.h>

#include "./types.hpp"
#include "./utl/json.hpp"

