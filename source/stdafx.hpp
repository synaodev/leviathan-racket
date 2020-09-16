#ifndef LEVIATHAN_INCLUDED_STDAFX_HPP
#define LEVIATHAN_INCLUDED_STDAFX_HPP

// C standard libraries
#include <cstdlib>
#include <cstddef>
#include <cstdarg>
#include <cstdint>
#include <cinttypes>
#include <cassert>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <clocale>
#include <cmath>
#include <cfloat>
#include <climits>
#include <ciso646>

// C++ standard libraries
#include <type_traits>
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
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <random>
#include <numeric>
#include <iosfwd>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <streambuf>
#include <locale>
#include <atomic>
#include <thread>
#include <mutex>
#include <future>

#if defined(__clang__) && defined(__apple_build_version__)
	#include <filesystem>
#else
	#include <ghc/filesystem.hpp>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>
#include <fmt/format.h>

#if defined(LEVIATHAN_EXECUTABLE_NAOMI)
	#if defined(LEVIATHAN_PLATFORM_LINUX) || defined(LEVIATHAN_USES_VCPKG)
		#include <AL/al.h>
		#include <AL/alc.h>
	#else
		#include <al.h>
		#include <alc.h>
	#endif
	#include <angelscript.h>
	#include <entt/entt.hpp>
	#include <tmxlite/Map.hpp>
	#include <tmxlite/TileLayer.hpp>
	#include <tmxlite/ImageLayer.hpp>
	#include <tmxlite/ObjectGroup.hpp>
	#include <pxtone/pxtnService.h>
#endif

#endif // LEVIATHAN_INCLUDED_STDAFX_HPP
