#include "./draw_fade.hpp"
#include "../sym/renderer.hpp"

static constexpr real_t kChange = 8.0f;

draw_fade_t::draw_fade_t() : 
	write(true),
	state(fade_state_t::DoneOut),
	bounding(0.0f, 0.0f, 0.0f, 0.0f)
{

}

void draw_fade_t::init() {
	this->reset();
}

void draw_fade_t::reset() {
	write = true;
	state = fade_state_t::DoneOut;
	bounding = rect_t(0.0f, 0.0f, 320.0f, 180.0f);
}

void draw_fade_t::handle() {
	switch (state) {
		case fade_state_t::DoneIn:
		case fade_state_t::DoneOut: {
			break;
		}
		case fade_state_t::FadingIn: {
			write = true;
			bounding.h -= kChange;
			if (bounding.h < 0.0f) {
				state = fade_state_t::DoneIn;
				bounding.h = 0.0f;
			}
			break;
		}
		case fade_state_t::FadingOut: {
			write = true;
			bounding.h += kChange;
			if (bounding.h > 180.0f) {
				state = fade_state_t::DoneOut;
				bounding.h = 180.0f;
			}
			break;
		}
	}
}

void draw_fade_t::render(renderer_t& renderer) const {
	auto& batch = renderer.get_overlay_quads(
		layer_value::HeadsUp,
		blend_mode_t::Alpha, 
		pipeline_t::VtxBlankColors
	);
	if (write) {
		write = false;
		batch.begin(quad_batch_t::SingleQuad)
			.vtx_blank_write(bounding, glm::vec4(0.0f, 0.0f, 0.125f, 1.0f))
		.end();
	}
	else {
		batch.skip(quad_batch_t::SingleQuad);
	}
}

void draw_fade_t::fade_in() {
	state = fade_state_t::FadingIn;
	bounding.h = 180.0f;
}

void draw_fade_t::fade_out() {
	state = fade_state_t::FadingOut;
	bounding.h = 0.0f;
}

bool draw_fade_t::is_done() const {
	return state == fade_state_t::DoneOut;
}

bool draw_fade_t::is_moving() const {
	return state == fade_state_t::FadingIn or state == fade_state_t::FadingOut;
}

bool draw_fade_t::is_visible() const {
	return state != fade_state_t::DoneIn;
}
