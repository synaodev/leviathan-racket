#include "./dialogue_gui.hpp"

#include "../utility/vfs.hpp"
#include "../event/receiver.hpp"
#include "../event/array.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/renderer.hpp"

#include "../resource/id.hpp"

static const glm::vec2 kDefaultRect = glm::vec2(256.0f, 56.0f);
static const glm::vec2 kFacesOffset = glm::vec2(10.0f, 4.0f);
static const glm::vec2 kArrowOffset = glm::vec2(5.0f, 10.0f);
static const glm::vec2 kTextOffsetA = glm::vec2(10.0f, 6.0f);
static const glm::vec2 kTextOffsetB = glm::vec2(68.0f, 6.0f);

static constexpr arch_t kFacesAnimState = 4;
static constexpr real64_t kDefaultDelay = 0.04;

dialogue_gui_t::dialogue_gui_t() :
	write(true),
	flags(0),
	cursor_index(0),
	cursor_total(0),
	timer(0.0),
	delay(kDefaultDelay),
	rect(glm::zero<glm::vec2>(), kDefaultRect),
	text(),
	faces(),
	arrow(),
	suspender()
{

}

bool dialogue_gui_t::init(receiver_t& receiver) {
	const font_t* font = vfs::font(0);
	const animation_t* animation = vfs::animation(res::anim::Heads);
	text.set_font(font);
	faces.set_file(animation);
	arrow.set_file(animation);
	arrow.set_state(1);
	suspender = [&receiver] {
		receiver.suspend();
	};
	return font != nullptr and animation != nullptr;
}

void dialogue_gui_t::reset() {
	this->close_textbox();
}

void dialogue_gui_t::handle(const input_t& input, audio_t& audio) {
	if (flags[dialogue_flag_t::Textbox]) {
		if (!text.finished()) {
			if (flags[dialogue_flag_t::Sound]) {
				flags[dialogue_flag_t::Sound] = false;
				flags[dialogue_flag_t::Writing] = true;
				text.increment();
				audio.play(res::sfx::Text, 7);
			}
		} else if (flags[dialogue_flag_t::Question]) {
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
				flags[dialogue_flag_t::Question] = false;
				audio.play(res::sfx::TitleBeg, 0);
			}
		} else {
			flags[dialogue_flag_t::Writing] = false;
		}
	}
}

void dialogue_gui_t::update(real64_t delta) {
	if (flags[dialogue_flag_t::Textbox]) {
		timer += delta;
		if (timer >= delay) {
			timer = glm::mod(timer, delay);
			if (!text.finished()) {
				flags[dialogue_flag_t::Sound] = true;
			}
		}
		if (flags[dialogue_flag_t::Facebox]) {
			if (!text.finished()) {
				faces.update(delta);
			} else {
				faces.set_frame(0);
			}
		}
		if (flags[dialogue_flag_t::Question]) {
			arrow.update(delta);
		}
	}
}

void dialogue_gui_t::render(renderer_t& renderer) const {
	if (flags[dialogue_flag_t::Textbox]) {
		if (flags[dialogue_flag_t::Facebox]) {
			faces.render(renderer);
		} else {
			faces.force();
		}
		if (flags[dialogue_flag_t::Question]) {
			arrow.render(renderer);
		} else {
			arrow.force();
		}
		text.render(renderer);
		auto& list = renderer.get_overlay_quads(
			layer_value::HeadsUp,
			blend_mode_t::Alpha,
			pipeline_t::VtxBlankColors
		);
		if (write) {
			write = false;
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
	write = true;
	flags[dialogue_flag_t::Textbox] = true;
	cursor_index = 0;
	cursor_total = 0;
	rect = rect_t(glm::vec2(32.0f, 8.0f), kDefaultRect);
	faces.set_position(rect.left_top() + kFacesOffset);
	arrow.set_position(rect.left_top() + kArrowOffset);
	text.set_position(
		flags[dialogue_flag_t::Facebox] ?
		rect.left_top() + kTextOffsetB :
		rect.left_top() + kTextOffsetA
	);
}

void dialogue_gui_t::open_textbox_low() {
	write = true;
	flags[dialogue_flag_t::Textbox] = true;
	cursor_index = 0;
	cursor_total = 0;
	rect = rect_t(glm::vec2(32.0f, 114.0f), kDefaultRect);
	faces.set_position(rect.left_top() + kFacesOffset);
	arrow.set_position(rect.left_top() + kArrowOffset);
	text.set_position(
		flags[dialogue_flag_t::Facebox] ?
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
	write = true;
	cursor_index = 0;
	cursor_total = 0;
	delay = kDefaultDelay;
	flags.reset();
	text.clear();
	text.set_params(0.0f);
	text.set_position(rect.left_top() + kTextOffsetA);
	faces.set_state(0);
	faces.set_direction(direction_t::Right);
}

void dialogue_gui_t::set_face(arch_t state, direction_t direction) {
	flags[dialogue_flag_t::Facebox] = true;
	cursor_index = 0;
	cursor_total = 0;
	text.set_position(rect.left_top() + kTextOffsetB);
	faces.set_position(rect.left_top() + kFacesOffset);
	faces.set_state(state + kFacesAnimState);
	faces.set_direction(direction);
	arrow.set_position(rect.left_top() + kArrowOffset);
}

void dialogue_gui_t::set_face() {
	flags[dialogue_flag_t::Facebox] = false;
	cursor_index = 0;
	cursor_total = 0;
	text.set_position(rect.left_top() + kTextOffsetA);
	faces.set_position(rect.left_top() + kFacesOffset);
	faces.set_state(0);
	faces.set_direction(direction_t::Right);
	arrow.set_position(rect.left_top() + kArrowOffset);
}

void dialogue_gui_t::ask_question(const CScriptArray* array) {
	if (array != nullptr and array->GetSize() > 0) {
		flags[dialogue_flag_t::Question] = true;
		flags[dialogue_flag_t::Writing] = false;
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

bool dialogue_gui_t::get_flag(dialogue_flag_t flag) const {
	return flags[flag];
}

arch_t dialogue_gui_t::get_answer() const {
	return cursor_index;
}