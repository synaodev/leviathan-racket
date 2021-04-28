#include "./dialogue-gui.hpp"

#include "../resource/id.hpp"
#include "../resource/vfs.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/receiver.hpp"
#include "../system/renderer.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"

#include <angelscript/scriptarray.h>
#include <glm/common.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/gtc/constants.hpp>

static const glm::vec2 kDefaultRect = glm::vec2(256.0f, 56.0f);
static const glm::vec2 kFacesOffset = glm::vec2(10.0f, 4.0f);
static const glm::vec2 kArrowOffset = glm::vec2(5.0f, 10.0f);
static const glm::vec2 kTextOffsetA = glm::vec2(10.0f, 6.0f);
static const glm::vec2 kTextOffsetB = glm::vec2(68.0f, 6.0f);

static constexpr real_t kKeyHeldDelay = constants::MinInterval<real_t>();
static constexpr real_t kDefaultDelay = constants::MinInterval<real_t>() * 2.8778f;
static constexpr real_t kHighestDelay = constants::MinInterval<real_t>() * 6.0f;

bool dialogue_gui_t::init(receiver_t& receiver) {
	const animation_t* faces_animation = vfs_t::animation(res::anim::Faces);
	const animation_t* heads_animation = vfs_t::animation(res::anim::Heads);
	if (!faces_animation or !heads_animation) {
		synao_log("Dialogue GUI is missing resources and cannot be rendered!\n");
		return false;
	}
	if (!this->refresh()) {
		return false;
	}
	faces.set_file(faces_animation);
	arrow.set_file(heads_animation);
	arrow.set_state(1);
	suspender = [&receiver] {
		receiver.suspend();
	};
	synao_log("Dialogue GUI is ready.\n");
	return true;
}

void dialogue_gui_t::reset() {
	this->close_textbox();
}

bool dialogue_gui_t::refresh() {
	const font_t* font = vfs_t::font(0);
	if (!font) {
		synao_log("Dialogue GUI is missing font and cannot be rendered!\n");
		return false;
	}
	text.set_font(font);
	return true;
}

void dialogue_gui_t::handle(const input_t& input, audio_t& audio) {
	if (flags[flags_t::Textbox]) {
		if (!text.finished()) {
			if (flags[flags_t::Sound]) {
				flags[flags_t::Sound] = false;
				text.increment();
				audio.play(res::sfx::Text, 7);
			}
			if (!flags[flags_t::Delay]) {
				delay = input.holding[btn_t::Yes] ? kKeyHeldDelay : kDefaultDelay;
			}
		} else if (flags[flags_t::Question]) {
			if (input.pressed[btn_t::Up]) {
				if (cursor_index > 0) {
					--cursor_index;
					arrow.mut_position(0.0f, -text.get_font_size().y);
					audio.play(res::sfx::Select, 0);
				}
			} else if (input.pressed[btn_t::Down]) {
				if (cursor_index < cursor_total) {
					++cursor_index;
					arrow.mut_position(0.0f, text.get_font_size().y);
					audio.play(res::sfx::Select, 0);
				}
			} else if (input.pressed[btn_t::Jump]) {
				cursor_total = 0;
				flags[flags_t::Question] = false;
				audio.play(res::sfx::TitleBeg, 0);
			}
		} else {
			flags[flags_t::Writing] = false;
		}
	}
}

void dialogue_gui_t::update(real64_t delta) {
	if (flags[flags_t::Textbox]) {
		timer += static_cast<real_t>(delta);
		if (timer >= delay) {
			timer = glm::mod(timer, delay);
			if (!text.finished()) {
				flags[flags_t::Writing] = true;
				flags[flags_t::Sound] = true;
			}
		}
		if (flags[flags_t::Facebox]) {
			if (!text.finished()) {
				faces.update(delta);
			} else {
				faces.set_frame(0);
			}
		}
		if (flags[flags_t::Question]) {
			arrow.update(delta);
		}
	}
}

void dialogue_gui_t::render(renderer_t& renderer) const {
	if (flags[flags_t::Textbox]) {
		if (flags[flags_t::Facebox]) {
			faces.render(renderer);
		} else {
			faces.invalidate();
		}
		if (flags[flags_t::Question]) {
			arrow.render(renderer);
		} else {
			arrow.invalidate();
		}
		text.render(renderer);
		auto& list = renderer.display_list(
			layer_value::Persistent,
			blend_mode_t::Alpha,
			program_t::Colors
		);
		if (amend) {
			amend = false;
			list.begin(display_list_t::SingleQuad)
				.vtx_blank_write(rect, glm::vec4(0.0f, 0.0f, 0.0f, 0.5f))
				.vtx_transform_write(rect.left_top())
			.end();
		} else {
			list.skip(display_list_t::SingleQuad);
		}
	}
}

void dialogue_gui_t::open_textbox_high() {
	amend = true;
	flags[flags_t::Textbox] = true;
	cursor_index = 0;
	cursor_total = 0;
	rect = rect_t { glm::vec2(32.0f, 8.0f), kDefaultRect };
	faces.set_position(rect.left_top() + kFacesOffset);
	arrow.set_position(rect.left_top() + kArrowOffset);
	text.set_position(
		flags[flags_t::Facebox] ?
		rect.left_top() + kTextOffsetB :
		rect.left_top() + kTextOffsetA
	);
}

void dialogue_gui_t::open_textbox_low() {
	amend = true;
	flags[flags_t::Textbox] = true;
	cursor_index = 0;
	cursor_total = 0;
	rect = rect_t { glm::vec2(32.0f, 114.0f), kDefaultRect };
	faces.set_position(rect.left_top() + kFacesOffset);
	arrow.set_position(rect.left_top() + kArrowOffset);
	text.set_position(
		flags[flags_t::Facebox] ?
		rect.left_top() + kTextOffsetB :
		rect.left_top() + kTextOffsetA
	);
}

void dialogue_gui_t::write_textbox(const std::string& string) {
	text.append_string(string, false);
}

void dialogue_gui_t::clear_textbox() {
	text.clear();
}

void dialogue_gui_t::close_textbox() {
	amend = true;
	cursor_index = 0;
	cursor_total = 0;
	timer = 0.0f;
	delay = kDefaultDelay;
	flags.reset();
	text.clear();
	text.set_color(glm::one<glm::vec4>());
	text.set_position(rect.left_top() + kTextOffsetA);
	faces.set_state(0);
	faces.set_direction(direction_t::Right);
}

void dialogue_gui_t::set_face(arch_t state, direction_t direction) {
	flags[flags_t::Facebox] = true;
	cursor_index = 0;
	cursor_total = 0;
	text.set_position(rect.left_top() + kTextOffsetB);
	faces.set_position(rect.left_top() + kFacesOffset);
	faces.set_state(state);
	faces.set_direction(direction);
	arrow.set_position(rect.left_top() + kArrowOffset);
}

void dialogue_gui_t::set_face() {
	flags[flags_t::Facebox] = false;
	cursor_index = 0;
	cursor_total = 0;
	text.set_position(rect.left_top() + kTextOffsetA);
	faces.set_position(rect.left_top() + kFacesOffset);
	faces.set_state(0);
	faces.set_direction(direction_t::Right);
	arrow.set_position(rect.left_top() + kArrowOffset);
}

void dialogue_gui_t::set_delay(real_t delay) {
	flags[flags_t::Delay] = true;
	this->delay = glm::clamp(delay, kDefaultDelay, kHighestDelay);
}

void dialogue_gui_t::set_delay() {
	flags[flags_t::Delay] = false;
	this->delay = kDefaultDelay;
}

void dialogue_gui_t::ask_question(const CScriptArray* array) {
	if (array and array->GetSize() > 0) {
		flags[flags_t::Question] = true;
		flags[flags_t::Writing] = false;
		text.set_string("  ");
		for (uint_t it = 0; it < array->GetSize(); ++it) {
			auto question = reinterpret_cast<const std::string*>(array->At(it));
			text.append_string(*question + "\n  ");
		}
		cursor_index = 0;
		cursor_total = static_cast<arch_t>(array->GetSize() - 1);
		std::invoke(suspender);
	}
}

bool dialogue_gui_t::get_flag(flags_t flag) const {
	return flags[flag];
}

arch_t dialogue_gui_t::get_answer() const {
	return cursor_index;
}
