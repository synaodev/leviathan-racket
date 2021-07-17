#include "./tilemap-parallax.hpp"
#include "./properties.hpp"

#include "../system/renderer.hpp"
#include "../video/texture.hpp"

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

namespace {
	constexpr byte_t kBoundsXProp[] = "rect.x";
	constexpr byte_t kBoundsYProp[] = "rect.y";
	constexpr byte_t kBoundsWProp[] = "rect.w";
	constexpr byte_t kBoundsHProp[] = "rect.h";
	constexpr byte_t kScrollXProp[] = "scroll.x";
	constexpr byte_t kScrollYProp[] = "scroll.y";
}

void tilemap_parallax_t::init(const std::unique_ptr<tmx::Layer>& layer, const glm::vec2& dimensions) {
	assert(layer);
	// Set dimensions
	glm::vec2 dim = dimensions;
	if (dim.x == 0.0f or dim.y == 0.0f) {
		dim = glm::one<glm::vec2>();
	}
	this->dimensions = dim;

	// Configure bounds/scrolling from properties
	this->bounding = { 0.0f, 0.0f, 1.0f, 1.0f };
	glm::vec2 inv = 1.0f / this->dimensions;
	for (auto&& property : layer->getProperties()) {
		auto& name = property.getName();
		if (name == kBoundsXProp) {
			this->bounding.x = ftcv::prop_to_real(property) * inv.x;
		} else if (name == kBoundsYProp) {
			this->bounding.y = ftcv::prop_to_real(property) * inv.y;
		} else if (name == kBoundsWProp) {
			this->dimensions.x = ftcv::prop_to_real(property);
			this->bounding.w = this->dimensions.x * inv.x;
		} else if (name == kBoundsHProp) {
			this->dimensions.y = ftcv::prop_to_real(property);
			this->bounding.h = this->dimensions.y * inv.y;
		} else if (name == kScrollXProp) {
			this->scrolling.x = ftcv::prop_to_real(property);
		} else if (name == kScrollYProp) {
			this->scrolling.y = ftcv::prop_to_real(property);
		}
	}
}

void tilemap_parallax_t::handle(const rect_t& viewport) {
	glm::vec2 next = glm::mod(
		viewport.left_top() * -scrolling,
		dimensions
	);
	if (position != next) {
		indices = 0;
		position = next;
	}
}

void tilemap_parallax_t::render(renderer_t& renderer, const rect_t& viewport, const texture_t* texture) const {
	auto& list = renderer.display_list(
		layer_value::Background,
		blend_mode_t::Alpha,
		program_t::Sprites
	);
	if (indices == 0) {
		sint_t texID = texture ? texture->get_name() : 0;
		for (real_t y = viewport.y + position.y - dimensions.y; y < viewport.bottom(); y += dimensions.y) {
			for (real_t x = viewport.x + position.x - dimensions.x; x < viewport.right(); x += dimensions.x) {
				list.begin(display_list_t::SingleQuad)
					.vtx_major_write(bounding, dimensions, mirroring_t::None, 1.0f, texID)
					.vtx_transform_write(x, y)
				.end();
				indices += display_list_t::SingleQuad;
			}
		}
	} else {
		list.skip(indices);
	}
}
