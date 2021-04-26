#include "./tilemap.hpp"
#include "./tileflag.hpp"
#include "./camera.hpp"
#include "./properties.hpp"

#include "../resource/vfs.hpp"
#include "../system/renderer.hpp"
#include "../utility/constants.hpp"

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/ImageLayer.hpp>

static constexpr sint_t kScreenWidth  = 21;
static constexpr sint_t kScreenHeight = 12;

void tilemap_t::reset() {
	amend = true;
	dimensions = glm::zero<glm::ivec2>();
	attributes.clear();
	previous_viewport = rect_t {
		-constants::TileDimensions<real_t>(),
		constants::NormalDimensions<real_t>()
	};
	layer_texture = nullptr;
	parallax_texture = nullptr;
	tilemap_parallaxes.clear();
	tilemap_layers.clear();
}

void tilemap_t::handle(const camera_t& camera) {
	const rect_t viewport = camera.get_viewport();
	for (auto&& parallax : tilemap_parallaxes) {
		parallax.handle(viewport);
	}
	if (!previous_viewport.round_compare(viewport)) {
		previous_viewport = viewport;
		amend = true;
		const glm::ivec2 first {
			glm::max(tilemap_t::floor(viewport.x), 0),
			glm::max(tilemap_t::floor(viewport.y), 0)
		};
		const glm::ivec2 last {
			glm::min(tilemap_t::ceiling(viewport.right() + constants::TileSize<real_t>()), dimensions.x),
			glm::min(tilemap_t::ceiling(viewport.bottom() + constants::TileSize<real_t>()), dimensions.y)
		};
		arch_t range = camera.get_tile_range(first, last);
		for (auto&& layer : tilemap_layers) {
			layer.handle(range, first, last, dimensions, layer_texture);
		}
	}
}

void tilemap_t::render(renderer_t& renderer, const rect_t& viewport) const {
	for (auto&& parallax : tilemap_parallaxes) {
		parallax.render(
			renderer,
			viewport,
			parallax_texture
		);
	}
	for (auto&& layer : tilemap_layers) {
		layer.render(renderer, amend);
	}
	amend = false;
}

void tilemap_t::push_properties(const tmx::Map& tmxmap) {
	// Resize according to tmxmap bounds
	const tmx::FloatRect bounds = tmxmap.getBounds();
	dimensions = {
		glm::max(static_cast<sint_t>(bounds.width) / constants::TileSize<sint_t>(), kScreenWidth),
		glm::max(static_cast<sint_t>(bounds.height) / constants::TileSize<sint_t>(), kScreenHeight)
	};
	attributes.resize(
		static_cast<arch_t>(dimensions.x) *
		static_cast<arch_t>(dimensions.y)
	);

	// Get tileset textures/attributes
	auto& tilesets = tmxmap.getTilesets();
	if (!tilesets.empty()) {
		const std::string name = ftcv::path_to_name(tilesets[0].getImagePath());
		layer_texture = vfs::texture(name);
		const std::string tilekey_path = vfs::resource_path(vfs_resource_path_t::TileKey);
		attribute_key = vfs::uint32_buffer(tilekey_path + name + ".attr");
	}
}

void tilemap_t::push_layer(const std::unique_ptr<tmx::Layer>& layer) {
	assert(layer);
	amend = true;
	if (!attribute_key.empty()) {
		const glm::vec2 inverse = layer_texture ?
			layer_texture->get_inverse_dimensions() :
			glm::zero<glm::vec2>();

		auto& recent = tilemap_layers.emplace_back(dimensions);
		recent.init(
			layer,
			inverse,
			attributes,
			attribute_key
		);
	}
}

void tilemap_t::push_parallax(const std::unique_ptr<tmx::Layer>& layer) {
	assert(layer);
	amend = true;
	auto& path = static_cast<tmx::ImageLayer*>(layer.get())->getImagePath();
	parallax_texture = vfs::texture(ftcv::path_to_name(path));

	const glm::vec2 parallax_dimensions = parallax_texture ?
		parallax_texture->get_dimensions() :
		glm::zero<glm::vec2>();
	auto& recent = tilemap_parallaxes.emplace_back();
	recent.init(layer, parallax_dimensions);
}

uint_t tilemap_t::get_attribute(sint_t x, sint_t y) const {
	if (x >= 0 and y >= 0 and x < dimensions.x and y < dimensions.y) {
		return attributes[
			static_cast<arch_t>(x) +
			static_cast<arch_t>(y) *
			static_cast<arch_t>(dimensions.x)
		];
	} else if (y > (dimensions.y + 1)) {
		return tileflag_t::OutBounds;
	}
	return tileflag_t::Empty;
}

uint_t tilemap_t::get_attribute(glm::ivec2 index) const {
	return this->get_attribute(index.x, index.y);
}

sint_t tilemap_t::round(real_t value) {
	return static_cast<sint_t>(value) / constants::TileSize<sint_t>();
}

sint_t tilemap_t::ceiling(real_t value) {
	return static_cast<sint_t>(glm::ceil(value / constants::TileSize<real_t>()));
}

sint_t tilemap_t::floor(real_t value) {
	return static_cast<sint_t>(glm::floor(value / constants::TileSize<real_t>()));
}

real_t tilemap_t::extend(sint_t value) {
	return static_cast<real_t>(value * constants::TileSize<sint_t>());
}
