#include "./wgt_input.hpp"

#include "../utl/vfs.hpp"
#include "../utl/setup_file.hpp"
#include "../sys/input.hpp"
#include "../sys/audio.hpp"

#include "../res.hpp"

static constexpr arch_t kTotalOptions = 11;
static const glm::vec2 kDefaultPosition = glm::vec2(4.0f, 2.0f);
static const glm::vec2 kAddingPositions = glm::vec2(3.0f, 16.0f);

wgt_input_t::wgt_input_t(arch_t flags) :
	widget_i(flags),
	cursor(0),
	header(),
	text(),
	arrow()
{

}

void wgt_input_t::init(const input_t& input, const video_t&, audio_t&, const music_t&, kernel_t&) {
	ready = true;
	header.set_font(vfs::font(0));
	header.set_position(kDefaultPosition);
	header.set_string(vfs::i18n_find("Input", 0));
	text.set_font(vfs::font(4));
	text.set_position(kDefaultPosition + kAddingPositions);
	this->setup_text(input);
	arrow.set_file(vfs::animation(res::anim::Heads));
	arrow.set_state(4);
	arrow.set_position(
		text.get_font_size().x, 
		-3.0f + (text.get_font_size().y * 2.0f)
	);
}

void wgt_input_t::handle(setup_file_t& config, input_t& input, video_t&, audio_t& audio, music_t&, kernel_t&, stack_gui_t&, draw_headsup_t&) {
	if (input.pressed[btn_t::Up]) {
		if (cursor > 0) {
			--cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, -text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Down]) {
		if (cursor < kTotalOptions) {
			++cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, text.get_font_size().y);
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
		text.render(renderer);
		arrow.render(renderer);
	}
}

void wgt_input_t::force() const {
	if (ready and active) {
		header.force();
		text.force();
		arrow.force();
	}
}

void wgt_input_t::setup_text(const input_t& input) {
	std::string data;
	for (arch_t it = 0; it < 12; ++it) {
		data += vfs::i18n_find("Input", it + 1);
		data += input.get_scancode_name(it);
	}
	text.set_string(data);
}