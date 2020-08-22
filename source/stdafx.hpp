#ifndef LEVIATHAN_INCLUDED_STDAFX_HPP
#define LEVIATHAN_INCLUDED_STDAFX_HPP

// C standard libraries
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cassert>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <clocale>
#include <cmath>
#include <cfloat>

// C++ standard libraries
#include <typeinfo>
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
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entity/registry.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <angelscript.h>
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>
#include <glad/glad.hpp>

#ifdef LEVIATHAN_TOOLCHAIN_APPLECLANG
	#include <filesystem>
#endif

#ifdef LEVIATHAN_USES_VCPKG
	#include <al.h>
	#include <alc.h>
#else
	#include <AL/al.h>
	#include <AL/alc.h>
#endif

#include "./types.hpp"
#include "./utility/setup_file.hpp"
#include "./resource/icon.hpp"
#include "./pxtone/pxtnService.h"
#include "./menu/widget.hpp"

#endif
