#include "./font.hpp"
#include "./vfs.hpp"

#include "../utility/logger.hpp"
#include "../video/texture.hpp"

#include <fstream>
#include <glm/gtc/constants.hpp>
#include <nlohmann/json.hpp>

const font_glyph_t font_t::kNullGlyph {};

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

	if (!glyphs.empty()) {
		synao_log("Warning! Tried to overwrite font!\n");
		return;
	}

	const std::string full_path = directory + name;
	std::ifstream ifs { full_path, std::ios::binary };

	if (ifs.is_open()) {
		nlohmann::json file = nlohmann::json::parse(ifs);
		auto block = file["font"];

		dimensions.x = std::stof(block["common"]["-base"].get<std::string>());
		dimensions.y = std::stof(block["common"]["-lineHeight"].get<std::string>());

		atlas = vfs::atlas(block["pages"]["page"]["-file"].get<std::string>());
		for (auto ot : block["chars"]["char"]) {
			char32_t id = std::stoi(ot["-id"].get<std::string>());
			const font_glyph_t glyph {
				std::stof(ot["-x"].get<std::string>()),
				std::stof(ot["-y"].get<std::string>()),
				std::stof(ot["-width"].get<std::string>()),
				std::stof(ot["-height"].get<std::string>()),
				std::stof(ot["-xoffset"].get<std::string>()),
				std::stof(ot["-yoffset"].get<std::string>()),
				std::stof(ot["-xadvance"].get<std::string>()),
				std::invoke(make_table, ot["-chnl"].get<std::string>())
			};
			glyphs[id] = glyph;
		}

		if (block.find("kernings") != block.end()) {
			for (auto ot : block["kernings"]["kerning"]) {
				char32_t first = std::stoi(ot["-first"].get<std::string>());
				char32_t second = std::stoi(ot["-second"].get<std::string>());
				auto key = std::pair{first, second};
				kernings[key] = std::stof(ot["-amount"].get<std::string>());
			}
		}
	} else {
		synao_log("Failed to load font from {}!\n", full_path);
	}
}

const font_glyph_t& font_t::glyph(char32_t code_point) const {
	auto it = glyphs.find(code_point);
	if (it == glyphs.end()) {
		return kNullGlyph;
	}
	return it->second;
}

real_t font_t::kerning(char32_t first, char32_t second) const {
	if (first == U'\0' or second == U'\0') {
		return 0.0f;
	}
	auto it = kernings.find(std::pair{first, second});
	if (it == kernings.end()) {
		return 0.0f;
	}
	return it->second;
}

const atlas_t* font_t::get_atlas() const {
	return atlas;
}

sint_t font_t::get_atlas_name() const {
	if (atlas) {
		return atlas->get_name();
	}
	return 0;
}

glm::vec2 font_t::get_inverse_dimensions() const {
	if (atlas) {
		return atlas->get_inverse_dimensions();
	}
	return glm::one<glm::vec2>();
}

glm::vec2 font_t::get_dimensions() const {
	return dimensions;
}
