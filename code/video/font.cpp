#include "./font.hpp"
#include "./texture.hpp"
#include "./palette.hpp"

#include "../utility/vfs.hpp"
#include "../utility/json.hpp"

#include <fstream>

font_t::font_t() :
	glyphs(),
	dimensions(0.0f),
	texture(nullptr),
	palette(nullptr)
{

}

font_t::font_t(font_t&& that) noexcept : font_t() {
	if (this != &that) {
		std::swap(glyphs, that.glyphs);
		std::swap(dimensions, that.dimensions);
		std::swap(texture, that.texture);
		std::swap(palette, that.palette);
	}
}

font_t& font_t::operator=(font_t&& that) noexcept {
	if (this != &that) {
		std::swap(glyphs, that.glyphs);
		std::swap(dimensions, that.dimensions);
		std::swap(texture, that.texture);
		std::swap(palette, that.palette);
	}
	return *this;
}

bool font_t::load(const std::string& directory, const std::string& full_path) {
	std::ifstream ifs(full_path, std::ios::binary);
	if (ifs.is_open()) {
		nlohmann::json file = nlohmann::json::parse(ifs);
		dimensions.x = std::stof(file["font"]["common"]["-base"].get<std::string>());
		dimensions.y = std::stof(file["font"]["common"]["-lineHeight"].get<std::string>());
		texture = vfs::texture({ file["font"]["pages"]["page"]["-file"].get<std::string>() }, directory);
		palette = vfs::palette(file["font"]["pages"]["page"]["-pllt"].get<std::string>(), directory);
		for (auto ot : file["font"]["chars"]["char"]) {
			char32_t id = std::stoi(ot["-id"].get<std::string>());
			const font_glyph_t glyph(
				std::stof(ot["-x"].get<std::string>()),
				std::stof(ot["-y"].get<std::string>()),
				std::stof(ot["-width"].get<std::string>()),
				std::stof(ot["-height"].get<std::string>()),
				std::stof(ot["-xoffset"].get<std::string>()),
				std::stof(ot["-yoffset"].get<std::string>()),
				std::stof(ot["-xadvance"].get<std::string>())
			);
			glyphs[id] = glyph;
		}
		return texture != nullptr and palette != nullptr;
	}
	return false;
}

const font_glyph_t& font_t::glyph(char32_t code_point) const {
	auto it = glyphs.find(code_point);
	if (it == glyphs.end()) {
		return glyphs.at(0);
	}
	return it->second;
}

const texture_t* font_t::get_texture() const {
	return texture;
}

const palette_t* font_t::get_palette() const {
	return palette;
}

glm::vec2 font_t::get_inverse_dimensions() const {
	if (texture != nullptr) {
		return texture->get_inverse_dimensions();
	}
	return glm::one<glm::vec2>();
}

glm::vec2 font_t::get_dimensions() const {
	return dimensions;
}

real_t font_t::convert_table(real_t index) const {
	if (palette != nullptr) {
		return palette->convert(index);
	}
	return 0.0f;
}
