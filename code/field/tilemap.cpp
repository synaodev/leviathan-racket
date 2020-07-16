#include "./tilemap.hpp"
#include "./tile_flag.hpp"

#include <tmxlite/Map.hpp>

#include "../system/camera.hpp"
#include "../system/renderer.hpp"
#include "../utility/vfs.hpp"

static constexpr byte_t kTileKeyPath[] = "./vfs/tilekey/";
static constexpr sint_t kScreenWidth   = 21;
static constexpr sint_t kScreenHeight  = 12;
static constexpr sint_t kSintTileSize  = 16;
static constexpr real_t kRealTileSize  = 16.0f;

tilemap_t::tilemap_t() :
	write(false),
	dimensions(0),
	attributes(),
	attribute_key(),
	previous_viewport(-kRealTileSize, -kRealTileSize, 320.0f, 180.0f),
	tilemap_layer_texture(nullptr),
	tilemap_layer_palette(nullptr),
	parallax_texture(nullptr),
	backgrounds(),
	tilemap_layers()
{

}

void tilemap_t::reset() {
	write = true;
	dimensions = glm::zero<glm::ivec2>();
	attributes.clear();
	previous_viewport = rect_t(-kRealTileSize, -kRealTileSize, 320.0f, 180.0f);
	tilemap_layer_texture = nullptr;
	tilemap_layer_palette = nullptr;
	parallax_texture = nullptr;
	backgrounds.clear();
	tilemap_layers.clear();
}

void tilemap_t::handle(const camera_t& camera) {
	rect_t viewport = camera.get_viewport();
	for (auto&& background : backgrounds) {
		background.handle(viewport);
	}
	if (!previous_viewport.cmp_round(viewport)) {
		previous_viewport = viewport;
		write = true;
		glm::ivec2 first = glm::ivec2(
			glm::max(tilemap_t::floor(viewport.x), 0),
			glm::max(tilemap_t::floor(viewport.y), 0)
		);
		glm::ivec2 last = glm::ivec2(
			glm::min(tilemap_t::ceiling(viewport.right() + kRealTileSize), dimensions.x),
			glm::min(tilemap_t::ceiling(viewport.bottom() + kRealTileSize), dimensions.y)
		);
		arch_t range = camera.get_tile_range(first, last);
		for (auto&& tilemap_layer : tilemap_layers) {
			tilemap_layer.handle(range, first, last, dimensions);
		}
	}
}

void tilemap_t::render(renderer_t& renderer, rect_t viewport) const {
	for (auto&& background : backgrounds) {
		background.render(
			renderer, 
			viewport, 
			parallax_texture
		);
	}
	for (auto&& tilemap_layer : tilemap_layers) {
		tilemap_layer.render(
			renderer, 
			write, 
			tilemap_layer_texture, 
			tilemap_layer_palette
		);
	}
	write = false;
}

static const byte_t kPaletteProperty[] = "indexed";

void tilemap_t::push_properties(const tmx::Map& tmxmap) {
	const tmx::FloatRect bounds = tmxmap.getBounds();
	dimensions = glm::ivec2(
		glm::max(static_cast<sint_t>(bounds.width) / kSintTileSize, kScreenWidth),
		glm::max(static_cast<sint_t>(bounds.height) / kSintTileSize, kScreenHeight)
	);
	attributes.resize(
		static_cast<arch_t>(dimensions.x) *
		static_cast<arch_t>(dimensions.y)
	);
	auto& tilesets = tmxmap.getTilesets();
	if (!tilesets.empty()) {
		auto& tileset = tilesets[0];
		for (auto&& property : tileset.getProperties()) {
			auto& name = property.getName();
			if (name == kPaletteProperty) {
				tilemap_layer_palette = vfs::palette(tileset.getName());
			}
		}
		tilemap_layer_texture = vfs::texture(tileset.getName());
		attribute_key = vfs::sint_buffer(kTileKeyPath + tileset.getName() + ".atr");
	}
	if (tilesets.size() > 1) {
		parallax_texture = vfs::texture(tilesets[1].getName());
	}
}

void tilemap_t::push_tile_layer(const std::unique_ptr<tmx::Layer>& layer) {
	write = true;
	if (!attribute_key.empty()) {
		glm::vec2 inverse = tilemap_layer_texture != nullptr ? 
			tilemap_layer_texture->get_inverse_dimensions() :
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

void tilemap_t::push_parallax_background(const std::unique_ptr<tmx::Layer>& layer) {
	write = true;
	glm::vec2 parallax_dimensions = parallax_texture != nullptr ?
		parallax_texture->get_dimensions() :
		glm::zero<glm::vec2>();
	auto& recent = backgrounds.emplace_back();
	recent.init(layer, parallax_dimensions);
}

sint_t tilemap_t::get_attribute(sint_t x, sint_t y) const {
	if (x >= 0 and y >= 0 and x < dimensions.x and y < dimensions.y) {
		return attributes[
			static_cast<arch_t>(x) +
			static_cast<arch_t>(y) *
			static_cast<arch_t>(dimensions.x)
		];
	} else if (y > (dimensions.y + 1)) {
		return tile_flag_t::OutBounds;
	}
	return tile_flag_t::Empty;
}

sint_t tilemap_t::get_attribute(glm::ivec2 index) const {
	return this->get_attribute(index.x, index.y);
}

sint_t tilemap_t::round(real_t value) {
	return static_cast<sint_t>(value) / kSintTileSize;
}

sint_t tilemap_t::ceiling(real_t value) {
	return static_cast<sint_t>(glm::ceil(value / kRealTileSize));
}

sint_t tilemap_t::floor(real_t value) {
	return static_cast<sint_t>(glm::floor(value / kRealTileSize));
}

real_t tilemap_t::extend(sint_t value) {
	return static_cast<real_t>(value * kSintTileSize);
}