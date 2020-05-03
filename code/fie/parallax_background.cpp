#include "./parallax_background.hpp"

#include "../utl/tmx_convert.hpp"
#include "../sym/renderer.hpp"

static const byte_t kBoundsXProp[] = "bounds_x";
static const byte_t kBoundsYProp[] = "bounds_y";
static const byte_t kBoundsWProp[] = "bounds_w";
static const byte_t kBoundsHProp[] = "bounds_h";
static const byte_t kScrollXProp[] = "scroll_x";
static const byte_t kScrollYProp[] = "scroll_y";
static const byte_t kSpeedXProp[]  = "speed_x";
static const byte_t kSpeedYProp[]  = "speed_y";

parallax_background_t::parallax_background_t() :
	indices(0),
	position(0.0f),
	scrolling(0.0f),
	offsets(0.0f),
	speed(0.0f),
	dimensions(1.0f),
	bounding()
{
	
}

parallax_background_t::parallax_background_t(parallax_background_t&& that) : parallax_background_t() {
	if (this != &that) {
		std::swap(indices, that.indices);
		std::swap(position, that.position);
		std::swap(scrolling, that.scrolling);
		std::swap(offsets, that.offsets);
		std::swap(speed, that.speed);
		std::swap(dimensions, that.dimensions);
		std::swap(bounding, that.bounding);
	}
}

parallax_background_t& parallax_background_t::operator=(parallax_background_t&& that) {
	if (this != &that) {
		std::swap(indices, that.indices);
		std::swap(position, that.position);
		std::swap(scrolling, that.scrolling);
		std::swap(offsets, that.offsets);
		std::swap(speed, that.speed);
		std::swap(dimensions, that.dimensions);
		std::swap(bounding, that.bounding);
	}
	return *this;
}

void parallax_background_t::init(const std::unique_ptr<tmx::Layer>& layer, glm::vec2 dimensions) {
	if (dimensions.x == 0.0f or dimensions.y == 0.0f) {
		dimensions = glm::one<glm::vec2>();
	}
	this->dimensions = dimensions;
	this->bounding = rect_t(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec2 inv = 1.0f / this->dimensions;
	for (auto&& property : layer->getProperties()) {
		auto& name = property.getName();
		if (name == kBoundsXProp) {
			this->bounding.x = tmx_convert::prop_to_real(property) * inv.x;
		} else if (name == kBoundsYProp) {
			this->bounding.y = tmx_convert::prop_to_real(property) * inv.y;
		} else if (name == kBoundsWProp) {
			this->dimensions.x = tmx_convert::prop_to_real(property);
			this->bounding.w = this->dimensions.x * inv.x;
		} else if (name == kBoundsHProp) {
			this->dimensions.y = tmx_convert::prop_to_real(property);
			this->bounding.h = this->dimensions.y * inv.y;
		} else if (name == kScrollXProp) {
			this->scrolling.x = tmx_convert::prop_to_real(property);
		} else if (name == kScrollYProp) {
			this->scrolling.y = tmx_convert::prop_to_real(property);
		} else if (name == kSpeedXProp) {
			this->speed.x = tmx_convert::prop_to_real(property);
		} else if (name == kSpeedYProp) {
			this->speed.y = tmx_convert::prop_to_real(property);
		}
	}
}

void parallax_background_t::handle(rect_t viewport) {
	if (speed != glm::zero<glm::vec2>()) {
		offsets = glm::mod(offsets + speed, dimensions);
	}
	glm::vec2 next = glm::mod(
		(viewport.left_top() * -scrolling) - offsets,
		dimensions
	);
	if (position != next) {
		indices = 0;
		position = next;
	}
}

void parallax_background_t::render(renderer_t& renderer, rect_t viewport, const texture_t* texture) const {
	auto& batch = renderer.get_normal_quads(
		layer_value::Parallax,
		blend_mode_t::Alpha,
		pipeline_t::VtxMajorSprites,
		texture,
		nullptr
	);
	if (indices == 0) {
		for (real_t y = position.y - dimensions.y; y < viewport.h; y += dimensions.y) {
			for (real_t x = position.x - dimensions.x; x < viewport.w; x += dimensions.x) {
				batch.begin(quad_batch_t::SingleQuad)
					.vtx_major_write(bounding, dimensions, 0.0f, 1.0f, mirroring_t::None)
					.vtx_transform_write(x, y)
				.end();
				indices += quad_batch_t::SingleQuad;
			}
		}	
	} else {
		batch.skip(indices);
	}
}