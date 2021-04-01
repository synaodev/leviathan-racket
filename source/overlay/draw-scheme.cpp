#include "./draw-scheme.hpp"

#include "../resource/animation.hpp"
#include "../system/renderer.hpp"

void draw_scheme_t::update(real64_t delta) {
	if (file) {
		file->update(
			delta,
			amend,
			state,
			timer,
			frame
		);
	}
}

void draw_scheme_t::render(renderer_t& renderer) const {
	if (file and visible) {
		file->render(
			renderer,
			amend,
			state,
			frame,
			direction,
			table,
			position
		);
	}
}

void draw_scheme_t::invalidate() const {
	amend = true;
}

void draw_scheme_t::set_file(const animation_t* file) {
	this->amend = true;
	this->file = file;
}

void draw_scheme_t::set_visible(bool_t visible) {
	if (this->visible != visible) {
		this->amend = true;
		this->visible = visible;
	}
}

void draw_scheme_t::set_state(arch_t state) {
	if (this->state != state and state != NonState) {
		this->amend = true;
		this->timer = 0.0;
		this->state = state;
		this->frame = 0;
	}
}

void draw_scheme_t::set_direction(direction_t direction) {
	if (this->direction != direction) {
		this->amend = true;
		this->direction = direction;
	}
}

void draw_scheme_t::set_frame(arch_t frame) {
	if (this->frame != frame) {
		this->amend = true;
		this->timer = 0.0;
		this->frame = frame;
	}
}

void draw_scheme_t::set_table(sint_t table) {
	if (this->table != table) {
		this->amend = true;
		this->table = table;
	}
}

void draw_scheme_t::set_position(real_t x, real_t y) {
	const glm::vec2 p { x, y };
	this->set_position(p);
}

void draw_scheme_t::set_position(const glm::vec2& position) {
	if (this->position != position) {
		this->amend = true;
		this->position = position;
	}
}

void draw_scheme_t::mut_position(real_t x, real_t y) {
	const glm::vec2 o { x, y };
	this->mut_position(o);
}

void draw_scheme_t::mut_position(const glm::vec2& offset) {
	this->amend = true;
	this->position += offset;
}

bool draw_scheme_t::is_finished() const {
	if (file) {
		return file->is_finished(state, frame, timer);
	}
	return true;
}

real64_t draw_scheme_t::get_timer() const {
	return timer;
}

arch_t draw_scheme_t::get_state() const {
	return state;
}

direction_t draw_scheme_t::get_direction() const {
	return direction;
}

arch_t draw_scheme_t::get_frame() const {
	return frame;
}

sint_t draw_scheme_t::get_table() const {
	return table;
}

const glm::vec2& draw_scheme_t::get_position() const {
	return position;
}
