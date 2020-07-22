#ifndef SYNAO_UTILITY_TMX_CONVERT_HPP
#define SYNAO_UTILITY_TMX_CONVERT_HPP

#include <vector>
#include <string>
#include <tmxlite/Property.hpp>

#include "./rect.hpp"
#include "./enums.hpp"

namespace tmx_convert {
	void prop_to_stats(const std::vector<tmx::Property>& properties, direction_t& direction, std::string& event, arch_t& flags, sint_t& idnum, arch_t& deter);
	bool_t prop_to_bool(const tmx::Property& property);
	sint_t prop_to_sint(const tmx::Property& property);
	real_t prop_to_real(const tmx::Property& property);
	direction_t prop_to_direction(const tmx::Property& property);
	glm::vec4 prop_to_color(const tmx::Property& property);
	glm::vec4 color_to_color(const tmx::Colour& color);
	glm::vec2 vec_to_vec(const tmx::Vector2f& position);
	std::string prop_to_string(const tmx::Property& property);
	std::string prop_to_path(const tmx::Property& property);
	std::string path_to_name(const std::string& path);
	rect_t rect_to_rect(const tmx::FloatRect& rect);
}

#endif // SYNAO_UTILITY_TMX_CONVERT_HPP