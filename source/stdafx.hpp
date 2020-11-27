#ifndef LEVIATHAN_INCLUDED_STDAFX_HPP
#define LEVIATHAN_INCLUDED_STDAFX_HPP

// Standard libraries
#include <functional>
#include <bitset>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>

#include <nlohmann/json.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ImageLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <pxtone/pxtnService.h>

#include "./resource/vfs.hpp"

#include "./utility/constants.hpp"
#include "./utility/enums.hpp"
#include "./utility/logger.hpp"
#include "./utility/rect.hpp"
#include "./utility/rng.hpp"
#include "./utility/setup-file.hpp"
#include "./utility/watch.hpp"

#endif // LEVIATHAN_INCLUDED_STDAFX_HPP
