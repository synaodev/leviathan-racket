#pragma once

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

#include "./resource/config.hpp"
#include "./resource/vfs.hpp"

#include "./utility/constants.hpp"
#include "./utility/enums.hpp"
#include "./utility/logger.hpp"
#include "./utility/rect.hpp"
#include "./utility/rng.hpp"
#include "./utility/watch.hpp"
