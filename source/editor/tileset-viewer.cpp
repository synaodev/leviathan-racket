#include "./tileset-viewer.hpp"

#ifdef LEVIATHAN_USES_META

#include "../resource/vfs.hpp"
#include "../system/input.hpp"
#include "../system/renderer.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"
#include "../video/texture.hpp"

#include <fstream>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

static constexpr arch_t kBitmaskLength = 256;
static const glm::vec2 kTilesetViewerPosition { 160.0f, 32.0f };

tileset_viewer_t::tileset_viewer_t() {
	const glm::vec2 dimensions = constants::TileDimensions<real_t>();
	cursor.w = dimensions.x;
	cursor.h = dimensions.y;
}

void tileset_viewer_t::reset() {
	amend = true;
	select = false;
	index = 0;
	for (auto&& bitmask : bitmasks) {
		bitmask = 0;
	}
	synao_log("Cleared!\n");
}

void tileset_viewer_t::handle(const input_t& input) {
	if (!file.empty() and texture) {
		if (select) {
			bool pressed = false;

			if (input.pressed[btn_t::Up]) {
				index = glm::max(index - constants::TileSize<arch_t>(), (arch_t)0);
				pressed = true;
			} else if (input.pressed[btn_t::Down]) {
				index = glm::min(index + constants::TileSize<arch_t>(), kBitmaskLength - 1);
				pressed = true;
			} else if (input.pressed[btn_t::Right]) {
				index = glm::min(index + 1, kBitmaskLength - 1);
				pressed = true;
			} else if (input.pressed[btn_t::Left]) {
				index = glm::max(index - 1, (arch_t)0);
				pressed = true;
			}

			if (pressed) {
				amend = true;
				index = glm::clamp(index, (arch_t)0, kBitmaskLength - 1);

				const glm::vec2 position {
					static_cast<real_t>(index % constants::TileSize<arch_t>()) * cursor.w,
					static_cast<real_t>(index / constants::TileSize<arch_t>()) * cursor.h
				};

				cursor.x = position.x + kTilesetViewerPosition.x;
				cursor.y = position.y + kTilesetViewerPosition.y;
			}
		}

		if (input.pressed[btn_t::Click]) {
			const rect_t bounds {
				kTilesetViewerPosition,
				texture->get_dimensions()
			};

			if (bounds.contains(input.position)) {
				const glm::vec2 mouse_position = input.position - kTilesetViewerPosition;
				const glm::ivec2 integral_coordinates {
					static_cast<sint_t>(mouse_position.x) / constants::TileSize<sint_t>(),
					static_cast<sint_t>(mouse_position.y) / constants::TileSize<sint_t>()
				};
				const glm::vec2 real_coordinates {
					static_cast<real_t>(integral_coordinates.x * constants::TileSize<sint_t>()),
					static_cast<real_t>(integral_coordinates.y * constants::TileSize<sint_t>())
				};

				synao_log(
					"({}, {})\n",
					integral_coordinates.x,
					integral_coordinates.y
				);

				amend = true;
				select = true;
				index = static_cast<arch_t>(integral_coordinates.x) + static_cast<arch_t>(integral_coordinates.y) * constants::TileSize<arch_t>();

				cursor.x = real_coordinates.x + kTilesetViewerPosition.x;
				cursor.y = real_coordinates.y + kTilesetViewerPosition.y;
			}
		} else if (input.pressed[btn_t::Options]) {
			amend = true;
			select = false;
			index = 0;
		}
	} else {
		amend = true;
		select = false;
		index = 0;
	}
}

void tileset_viewer_t::update(real64_t delta) {
	if (select) {
		timer -= delta;
		if (timer <= 0.0) {
			amend = true;
			timer = constants::MinInterval() * 3.0;
			flash = !flash;
		}
	}
}

void tileset_viewer_t::render(renderer_t& renderer) const {
	if (amend) {
		amend = false;

		if (select and flash) {
			const rect_t raster { glm::zero<glm::vec2>(), cursor.dimensions() };

			auto& list = renderer.display_list(
				layer_value::Persistent,
				blend_mode_t::Alpha,
				program_t::Colors
			);
			list.begin(display_list_t::SingleQuad)
				.vtx_blank_write(
					raster,
					glm::one<glm::vec4>()
				)
				.vtx_transform_write(cursor.left_top())
			.end();
		}
		if (texture) {
			const rect_t raster { glm::zero<glm::vec2>(), glm::one<glm::vec2>() };

			auto& list = renderer.display_list(
				layer_value::Automatic,
				blend_mode_t::Alpha,
				program_t::Sprites
			);
			list.begin(display_list_t::SingleQuad)
				.vtx_major_write(
					raster,
					texture->get_dimensions(),
					mirroring_t::None,
					1.0f, texture->get_name(), 0
				)
				.vtx_transform_write(kTilesetViewerPosition)
			.end();
		}
	} else {
		if (select and flash) {
			auto& list = renderer.display_list(
				layer_value::Persistent,
				blend_mode_t::Alpha,
				program_t::Colors
			);
			list.skip(display_list_t::SingleQuad);
		}
		if (texture) {
			auto& list = renderer.display_list(
				layer_value::Automatic,
				blend_mode_t::Alpha,
				program_t::Sprites
			);
			list.skip(display_list_t::SingleQuad);
		}
	}
}

bool tileset_viewer_t::load(const std::string& path, renderer_t& renderer) {
	amend = true;
	select = false;

	if (file != path) {
		texture = vfs_t::texture(path);
		if (!texture) {
			return false;
		}

		const std::string tilekey_path = vfs_t::resource_path(vfs_resource_path_t::TileKey);
		bitmasks = vfs_t::uint32_buffer(tilekey_path + path + ".attr");
		if (bitmasks.empty()) {
			return false;
		}

		file = path;
		renderer.clear();
		synao_log("Load successful!\n");

		return true;
	}
	return false;
}

bool tileset_viewer_t::save() {
	select = false;

	if (!file.empty()) {
		const std::string tilekey_path = vfs_t::resource_path(vfs_resource_path_t::TileKey);
		std::ofstream ofs { tilekey_path + file + ".attr", std::ios::binary };

		if (ofs.is_open()) {
			ofs.write(
				reinterpret_cast<byte_t*>(bitmasks.data()),
				sizeof(uint_t) * kBitmaskLength
			);
			synao_log("Save successful!\n");
			return true;
		} else {
			synao_log("Error! Couldn't save attribute file {}!\n", file);
		}
	} else {
		synao_log("Error! Can't save file!\n");
	}
	return false;
}

bool tileset_viewer_t::selected() const {
	return select;
}

void tileset_viewer_t::set_bitmask(uint_t bitmask) {
	bitmasks[index] = bitmask;
}

uint_t tileset_viewer_t::get_bitmask() const {
	return bitmasks[index];
}

#endif
