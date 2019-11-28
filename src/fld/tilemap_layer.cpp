#include "./tilemap_layer.hpp"

#include <tmxlite/TileLayer.hpp>

#include "../utl/tmx_convert.hpp"
#include "../sys/renderer.hpp"

static constexpr arch_t kMinimumVerts = 21 * 13 * quad_batch_t::SingleQuad;
static constexpr real_t kRealTileSize = 16.0f;
static constexpr sint_t kSintTileSize = 16;
static constexpr sint_t kInvalidTiles = -1;
static const byte_t kCollideLayer[] = "collide";
static const byte_t kPriorityType[] = "priority";

tilemap_layer_t::tilemap_layer_t(glm::ivec2 map_size) : tilemap_layer_t() {
	tiles.resize(
		static_cast<arch_t>(map_size.x) * 
		static_cast<arch_t>(map_size.y)
	);
	quads.setup<vtx_major_t>();
	quads.resize(kMinimumVerts);
}

tilemap_layer_t::tilemap_layer_t() :
	priority(layer_value::TileBack),
	indices(0),
	inverse_dimensions(1.0f),
	tiles(),
	quads()
{
	quads.setup<vtx_major_t>();
}

tilemap_layer_t::tilemap_layer_t(tilemap_layer_t&& that) : tilemap_layer_t() {
	if (this != &that) {
		std::swap(priority, that.priority);
		std::swap(indices, that.indices);
		std::swap(inverse_dimensions, that.inverse_dimensions);
		std::swap(tiles, that.tiles);
		std::swap(quads, that.quads);
	}
}

tilemap_layer_t& tilemap_layer_t::operator=(tilemap_layer_t&& that) {
	if (this != &that) {
		std::swap(priority, that.priority);
		std::swap(indices, that.indices);
		std::swap(inverse_dimensions, that.inverse_dimensions);
		std::swap(tiles, that.tiles);
		std::swap(quads, that.quads);
	}
	return *this;
}

void tilemap_layer_t::init(const std::unique_ptr<tmx::Layer>& layer, glm::vec2 inverse_dimensions, std::vector<sint_t>& attributes, const std::vector<sint_t>& attribute_key) {
	if (inverse_dimensions.x == 0.0f or inverse_dimensions.y == 0.0f) {
		inverse_dimensions = glm::one<glm::vec2>();
	}
	this->inverse_dimensions = inverse_dimensions;
	bool colliding = false;
	for (auto&& property : layer->getProperties()) {
		auto& name = property.getName();
		if (name == kCollideLayer) {
			colliding = tmx_convert::prop_to_bool(property);
			priority = layer_value::TileFront;
		} else if (name == kPriorityType) {
			if (tmx_convert::prop_to_bool(property)) {
				priority = layer_value::TileFront;
			}
		}
	}
	auto& array = dynamic_cast<tmx::TileLayer*>(layer.get())->getTiles();
	for (arch_t it = 0; it < array.size(); ++it) {
		sint_t type = static_cast<sint_t>(array[it].ID) - 1;
		tiles[it] = type >= 0 ? 
			glm::ivec2(type % kSintTileSize, type / kSintTileSize) : 
			glm::ivec2(kInvalidTiles);
		if (colliding and type >= 0) {
			attributes[it] = attribute_key[type];
		}
	}
}

void tilemap_layer_t::handle(arch_t range, glm::ivec2 first, glm::ivec2 last, glm::ivec2 map_size) {
	if (range != quads.size()) {
		quads.resize(range);
	}
	indices = 0;
	glm::vec2 pos = glm::vec2(first * kSintTileSize);
	glm::vec2 uvs = glm::zero<glm::vec2>();
	for (sint_t y = first.y; y < last.y; ++y) {
		for (sint_t x = first.x; x < last.x; ++x) {
			arch_t index = static_cast<arch_t>(x) + static_cast<arch_t>(y) * static_cast<arch_t>(map_size.x);
			glm::ivec2 tile = tiles[index];
			if (tile.x >= 0 and tile.y >= 0) {
				uvs = glm::vec2(tile * kSintTileSize);
				vtx_major_t* quad = quads.at<vtx_major_t>(indices * quad_batch_t::SingleQuad);
				quad[0].position = pos;
				quad[0].uvcoords = uvs * inverse_dimensions;
				quad[0].table = 0.0f;
				quad[0].alpha = 1.0f;

				quad[1].position = glm::vec2(pos.x, pos.y + kRealTileSize);
				quad[1].uvcoords = glm::vec2(uvs.x, uvs.y + kRealTileSize) * inverse_dimensions;
				quad[1].table = 0.0f;
				quad[1].alpha = 1.0f;

				quad[2].position = glm::vec2(pos.x + kRealTileSize, pos.y);
				quad[2].uvcoords = glm::vec2(uvs.x + kRealTileSize, uvs.y) * inverse_dimensions;
				quad[2].table = 0.0f;
				quad[2].alpha = 1.0f;

				quad[3].position = glm::vec2(pos.x + kRealTileSize, pos.y + kRealTileSize);
				quad[3].uvcoords = glm::vec2(uvs.x + kRealTileSize, uvs.y + kRealTileSize) * inverse_dimensions;
				quad[3].table = 0.0f;
				quad[3].alpha = 1.0f;
				++indices;
			}
			pos.x += kRealTileSize;
		}
		pos.x = static_cast<real_t>(first.x * kSintTileSize);
		pos.y += kRealTileSize;
	}
}

void tilemap_layer_t::render(renderer_t& renderer, bool_t write, const texture_t* texture, const palette_t* palette) const {
	auto& batch = renderer.get_normal_quads(
		priority,
		blend_mode_t::Alpha,
		palette != nullptr ? render_pass_t::VtxMajorIndexed : render_pass_t::VtxMajorSprites,
		texture,
		palette
	);
	if (write) {
		batch.begin(indices * quad_batch_t::SingleQuad)
			.vtx_pool_write(&quads)
		.end();
	} else {
		batch.skip(indices * quad_batch_t::SingleQuad);
	}
}