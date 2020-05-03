#include "./wgt_input.hpp"

#include "../utl/vfs.hpp"
#include "../utl/setup_file.hpp"
#include "../sym/input.hpp"
#include "../sym/audio.hpp"

#include "../res/id.hpp"

static constexpr arch_t kTotalOptionsA = 11;
static constexpr arch_t kTotalOptionsB = 7;
static constexpr real64_t kTotalDelays = 0.32;
static const glm::vec2 kDefaultPosition = glm::vec2(4.0f, 2.0f);
static const glm::vec2 kAddingPositions = glm::vec2(3.0f, 16.0f);
static const glm::vec2 kRightsPositions = glm::vec2(175.0f, 16.0f);

wgt_input_t::wgt_input_t(arch_t flags) :
	widget_i(flags),
	siding(false),
	waiting(false),
	flashed(false),
	cursor(0),
	header(),
	left_text(),
	right_text(),
	arrow()
{

}

void wgt_input_t::init(const input_t& input, const video_t&, audio_t&, const music_t&, kernel_t&) {
	ready = true;
	header.set_font(vfs::font(0));
	header.set_position(kDefaultPosition);
	header.set_string(vfs::i18n_find("Input", 0));
	left_text.set_font(vfs::font(4));
	left_text.set_position(kDefaultPosition + kAddingPositions);
	right_text.set_font(vfs::font(4));
	right_text.set_position(kDefaultPosition + kRightsPositions);
	this->setup_text(input);
	arrow.set_file(vfs::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		left_text.get_font_size().x, 
		(left_text.get_font_size().y * 2.0f) - 3.0f
	);
}

void wgt_input_t::handle(setup_file_t& config, input_t& input, video_t&, audio_t& audio, music_t&, kernel_t&, stack_gui_t&, draw_headsup_t&) {
	if (waiting) {
		flashed = !flashed;
		if (input.has_valid_recording()) {
			const std::string name = input.get_config_name(cursor, siding);
			sint_t code = input.receive_record();
			if (siding) {
				btn_t other = input.set_joystick_binding(code, cursor);
				if (other == btn_t::Total) {
					config.set("Input", name, code);
				} else {
					config.swap("Input", name, input.get_config_name(other, siding));
				}
			} else {
				btn_t other = input.set_keyboard_binding(code, cursor);
				if (other == btn_t::Total) {
					config.set("Input", name, code);
				} else {
					config.swap("Input", name, input.get_config_name(other, siding));
				}
			}
			waiting = false;
			flashed = false;
			audio.play(res::sfx::TitleBeg, 9);
			this->setup_text(input);
		} else if (!input.has_joystick_connection() and siding) {
			waiting = false;
			flashed = false;
			input.set_nothings_recording();
			audio.play(res::sfx::Inven, 0);
		}
	} else if (input.pressed[btn_t::Up]) {
		if (cursor > 0) {
			--cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, -left_text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Down]) {
		const arch_t comparison = siding ? kTotalOptionsB : kTotalOptionsA;
		if (cursor < comparison) {
			++cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, left_text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Right]) {
		if (!siding) {
			siding = true;
			audio.play(res::sfx::Select, 0);
			if (cursor > kTotalOptionsB) {
				cursor = kTotalOptionsB;
				const glm::vec2 position = arrow.get_position();
				arrow.set_position(
					position.x + (kRightsPositions.x - 3.0f),
					(static_cast<real_t>(kTotalOptionsB) * left_text.get_font_size().y) + 
					((left_text.get_font_size().y * 2.0f) - 3.0f)
				);
			} else {
				arrow.mut_position(kRightsPositions.x - 3.0f, 0.0f);
			}
		}
	} else if (input.pressed[btn_t::Left]) {
		if (siding) {
			siding = false;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(-kRightsPositions.x + 3.0f, 0.0f);
		}
	} else if (input.pressed[btn_t::Yes]) {
		if (siding) {
			if (input.has_joystick_connection()) {
				waiting = true;
				flashed = false;
				input.set_joystick_recording();
				audio.play(res::sfx::Inven, 0);
			}
		} else {
			waiting = true;
			flashed = false;
			input.set_keyboard_recording();
			audio.play(res::sfx::Inven, 0);
		}
	} else if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
	}
	if (!active) {
		input.pressed.reset();
		audio.play(res::sfx::Inven, 0);
	}
}

void wgt_input_t::update(real64_t delta) {
	if (ready and active) {
		arrow.update(delta);
	}
}

void wgt_input_t::render(renderer_t& renderer) const {
	if (ready and active) {
		header.render(renderer);
		left_text.render(renderer);
		right_text.render(renderer);
		if (waiting) {
			if (flashed) {
				arrow.force();
			} else {
				arrow.render(renderer);
			}
		} else {
			arrow.render(renderer);
		}
	}
}

void wgt_input_t::force() const {
	if (ready and active) {
		header.force();
		left_text.force();
		right_text.force();
		arrow.force();
	}
}

void wgt_input_t::setup_text(const input_t& input) {
	std::string data;
	for (arch_t it = 0; it < 12; ++it) {
		data += vfs::i18n_find("Input", it + 1);
		data += input.get_scancode_name(it);
	}
	left_text.set_string(data);
	data.clear();
	for (arch_t it = 0; it < 8; ++it) {
		data += vfs::i18n_find("Input", it + 1);
		data += input.get_joystick_button(it);
	}
	right_text.set_string(data);
}