#include "./tilemap-layer.hpp"
#include "./properties.hpp"

#include "../system/renderer.hpp"
#include "../utility/constants.hpp"
#include "../video/texture.hpp"

#include <glm/gtc/constants.hpp>
#include <tmxlite/TileLayer.hpp>

static constexpr arch_t kMinimumVerts = 21 * 13 * display_list_t::SingleQuad;
static constexpr sint_t kInvalidTiles = -1;
static constexpr byte_t kCollideLayer[] = "collide";
static constexpr byte_t kPriorityType[] = "priority";

tilemap_layer_t::tilemap_layer_t(const glm::ivec2& dimensions) : tilemap_layer_t() {
	tiles.resize(
		static_cast<arch_t>(dimensions.x) *
		static_cast<arch_t>(dimensions.y)
	);
	quads.resize(kMinimumVerts);
}

tilemap_layer_t::tilemap_layer_t() {
	vertex_spec_t specify = vertex_spec_t::from(vtx_major_t::name());
	quads.setup(specify);
}

void tilemap_layer_t::init(const std::unique_ptr<tmx::Layer>& layer, const glm::vec2& inverse_dimensions, std::vector<uint_t>& attributes, const std::vector<uint_t>& attribute_key) {
	assert(layer);
	// Set dimensions
	glm::vec2 inv = inverse_dimensions;
	if (inv.x == 0.0f or inv.y == 0.0f) {
		inv = glm::one<glm::vec2>();
	}
	this->inverse_dimensions = inv;

	// Find the collision layer
	bool colliding = false;
	for (auto&& property : layer->getProperties()) {
		auto& name = property.getName();
		if (name == kCollideLayer) {
			colliding = ftcv::prop_to_bool(property);
			priority = layer_value::Foreground;
		} else if (name == kPriorityType) {
			if (ftcv::prop_to_bool(property)) {
				priority = layer_value::Foreground;
			}
		}
	}

	// Populate tile array
	auto& array = static_cast<tmx::TileLayer*>(layer.get())->getTiles();
	for (arch_t it = 0; it < array.size(); ++it) {
		sint_t type = static_cast<sint_t>(array[it].ID) - 1;
		tiles[it] = type >= 0 ?
			glm::ivec2 { type % constants::TileSize<sint_t>(), type / constants::TileSize<sint_t>() } :
			glm::ivec2 { kInvalidTiles };
		if (colliding and type >= 0) {
			attributes[it] = attribute_key[type];
		}
	}
}

void tilemap_layer_t::handle(arch_t range, const glm::ivec2& first, const glm::ivec2& last, const glm::ivec2& dimensions, const texture_t* texture) {
	if (range != quads.size()) {
		quads.resize(range);
	}

	indices = 0;
	glm::vec2 pos { first * constants::TileSize<sint_t>() };
	glm::vec2 uvs {};
	sint_t texID = texture ? texture->get_name() : 0;

	for (sint_t y = first.y; y < last.y; ++y) {
		for (sint_t x = first.x; x < last.x; ++x) {
			arch_t index = static_cast<arch_t>(x) + static_cast<arch_t>(y) * static_cast<arch_t>(dimensions.x);
			glm::ivec2 tile = tiles[index];

			if (tile.x >= 0 and tile.y >= 0) {
				uvs = glm::vec2(tile * constants::TileSize<sint_t>());

				vtx_major_t* quad = quads.at<vtx_major_t>(indices * display_list_t::SingleQuad);
				quad[0].position = pos;
				quad[0].matrix = 1;
				quad[0].uvcoords = uvs * inverse_dimensions;
				quad[0].alpha = 1.0f;
				quad[0].texID = texID;

				quad[1].position = { pos.x, pos.y + constants::TileSize<real_t>() };
				quad[1].matrix = 1;
				quad[1].uvcoords = glm::vec2(uvs.x, uvs.y + constants::TileSize<real_t>()) * inverse_dimensions;
				quad[1].alpha = 1.0f;
				quad[1].texID = texID;

				quad[2].position = { pos.x + constants::TileSize<real_t>(), pos.y };
				quad[2].matrix = 1;
				quad[2].uvcoords = glm::vec2(uvs.x + constants::TileSize<real_t>(), uvs.y) * inverse_dimensions;
				quad[2].alpha = 1.0f;
				quad[2].texID = texID;

				quad[3].position = { pos.x + constants::TileSize<real_t>(), pos.y + constants::TileSize<real_t>() };
				quad[3].matrix = 1;
				quad[3].uvcoords = glm::vec2(uvs.x + constants::TileSize<real_t>(), uvs.y + constants::TileSize<real_t>()) * inverse_dimensions;
				quad[3].alpha = 1.0f;
				quad[3].texID = texID;

				++indices;
			}
			pos.x += constants::TileSize<real_t>();
		}
		pos.x = static_cast<real_t>(first.x * constants::TileSize<sint_t>());
		pos.y += constants::TileSize<real_t>();
	}
}

void tilemap_layer_t::render(renderer_t& renderer, bool_t amend) const {
	auto& list = renderer.display_list(
		priority,
		blend_mode_t::Alpha,
		program_t::Sprites
	);
	if (amend) {
		list.begin(indices * display_list_t::SingleQuad)
			.vtx_pool_write(quads)
		.end();
	} else {
		list.skip(indices * display_list_t::SingleQuad);
	}
}
