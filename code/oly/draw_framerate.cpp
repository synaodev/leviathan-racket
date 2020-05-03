#include "./draw_framerate.hpp"

static const real64_t kDelayTime = 0.2;

draw_framerate_t::draw_framerate_t() :
	timer(0.0),
	count()
{

}

void draw_framerate_t::init(const texture_t* texture, const palette_t* palette) {
	count.set_texture(texture);
	count.set_palette(palette);
	count.set_bounding(56.0f, 18.0f, 8.0f, 10.0f);
	count.set_position(310.0f, 168.0f);
	count.set_visible(true);
	count.set_backwards(true);
	count.set_layer(layer_value::Invisible);
}

void draw_framerate_t::update(real64_t delta) {
	if (delta != 0.0) {
		timer += delta;
		if (timer >= kDelayTime) {
			count.set_value(static_cast<sint_t>(1.0 / delta));
			timer = glm::mod(timer, kDelayTime);
		}
	}
}

void draw_framerate_t::render(renderer_t& renderer) const {
	count.render(renderer);
}

void draw_framerate_t::force() const {
	count.force();
}

void draw_framerate_t::set_position(real_t x, real_t y) {
	count.set_position(x, y);
}

void draw_framerate_t::set_position(glm::vec2 position) {
	count.set_position(position);
}
