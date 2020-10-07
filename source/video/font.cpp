#include "./font.hpp"
#include "./texture.hpp"

#include "../utility/vfs.hpp"
#include "../utility/logger.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

font_t::font_t() :
	glyphs(),
	dimensions(0.0f),
	texture(nullptr)
{

}

font_t::font_t(font_t&& that) noexcept : font_t() {
	if (this != &that) {
		std::swap(glyphs, that.glyphs);
		std::swap(dimensions, that.dimensions);
		std::swap(texture, that.texture);
	}
}

font_t& font_t::operator=(font_t&& that) noexcept {
	if (this != &that) {
		std::swap(glyphs, that.glyphs);
		std::swap(dimensions, that.dimensions);
		std::swap(texture, that.texture);
	}
	return *this;
}

void font_t::load(const std::string& directory, const std::string& name) {
	auto make_table = [](const std::string& value) {
		switch (std::stoi(value)) {
		case 1: return 2;
		case 2: return 1;
		case 4: return 0;
		case 8: return 3;
		default: return 0;
		}
	};
	if (glyphs.size() > 0) {
		synao_log("Warning! Tried to overwrite font!\n");
		return;
	}
	const std::string full_path = directory + name;
	std::ifstream ifs(full_path, std::ios::binary);
	if (ifs.is_open()) {
		nlohmann::json file = nlohmann::json::parse(ifs);
		dimensions.x = std::stof(file["font"]["common"]["-base"].get<std::string>());
		dimensions.y = std::stof(file["font"]["common"]["-lineHeight"].get<std::string>());
		texture = vfs::texture({ file["font"]["pages"]["page"]["-file"].get<std::string>() }, directory);
		for (auto ot : file["font"]["chars"]["char"]) {
			char32_t id = std::stoi(ot["-id"].get<std::string>());
			const font_glyph_t glyph(
				std::stof(ot["-x"].get<std::string>()),
				std::stof(ot["-y"].get<std::string>()),
				std::stof(ot["-width"].get<std::string>()),
				std::stof(ot["-height"].get<std::string>()),
				std::stof(ot["-xoffset"].get<std::string>()),
				std::stof(ot["-yoffset"].get<std::string>()),
				std::stof(ot["-xadvance"].get<std::string>()),
				make_table(ot["-chnl"].get<std::string>())
			);
			glyphs[id] = glyph;
		}
	} else {
		synao_log("Failed to load font from {}!\n", full_path);
	}
}

const font_glyph_t& font_t::glyph(char32_t code_point) const {
	static const font_glyph_t kInvalidGlyph = font_glyph_t();
	auto it = glyphs.find(code_point);
	if (it == glyphs.end()) {
		return kInvalidGlyph;
	}
	return it->second;
}

const texture_t* font_t::get_texture() const {
	return texture;
}

sint_t font_t::get_texture_name() const {
	if (texture != nullptr) {
		return texture->get_name();
	}
	return 0;
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
