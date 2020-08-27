#include "./draw_hidden.hpp"

#ifdef LEVIATHAN_USES_META

static constexpr real64_t kFramerateDelay = 0.2;

draw_hidden_t::draw_hidden_t() :
	state(draw_hidden_state_t::None),
	radio(),
	timer(0.0),
	count(),
	text()
{

}

void draw_hidden_t::init(const texture_t* texture, const palette_t* palette, const font_t* font) {
	// Counter
	count.set_texture(texture);
	count.set_palette(palette);
	count.set_bounding(56.0f, 18.0f, 8.0f, 10.0f);
	count.set_position(310.0f, 168.0f);
	count.set_visible(true);
	count.set_backwards(true);
	count.set_layer(layer_value::Invisible);
	// Text
	text.set_font(font);
	text.set_layer(layer_value::Invisible);
}

void draw_hidden_t::update(real64_t delta) {
	switch (state) {
	case draw_hidden_state_t::None:
		break;
	case draw_hidden_state_t::Framerate:
		if (delta != 0.0) {
			timer += delta;
			if (timer >= kFramerateDelay) {
				count.set_value(static_cast<sint_t>(1.0 / delta));
				timer = glm::mod(timer, kFramerateDelay);
			}
		}
		break;
	case draw_hidden_state_t::DrawCalls:
	case draw_hidden_state_t::ActorCount:
		if (radio != nullptr) {
			sint_t value = std::invoke(radio);
			count.set_value(value);
		}
		break;
	default:
		break;
	}
}

void draw_hidden_t::render(renderer_t& renderer) const {
	if (state != draw_hidden_state_t::None) {
		count.render(renderer);
		text.render(renderer);
	}
}

void draw_hidden_t::invalidate() const {
	if (state != draw_hidden_state_t::None) {
		count.invalidate();
		text.invalidate();
	}
}

void draw_hidden_t::set_state(draw_hidden_state_t state, std::function<sint_t()> radio) {
	this->state = state;
	this->radio = radio;
	switch (state) {
	case draw_hidden_state_t::None:
		break;
	case draw_hidden_state_t::Framerate:
		text.set_position(292.0f, 154.0f);
		text.set_string("FPS:");
		break;
	case draw_hidden_state_t::DrawCalls:
		text.set_position(245.0f, 154.0f);
		text.set_string("Draw Calls:");
		break;
	case draw_hidden_state_t::ActorCount:
		text.set_position(273.0f, 154.0f);
		text.set_string("Actors:");
		break;
	default:
		break;
	}
	if (this->radio != nullptr) {
		sint_t value = std::invoke(this->radio);
		count.set_value(value);
	} else {
		count.set_value(0);
	}
}

#endif
