#include "./wgt_video.hpp"

#include "../utl/vfs.hpp"
#include "../utl/setup_file.hpp"
#include "../sym/input.hpp"
#include "../sym/video.hpp"
#include "../sym/audio.hpp"

#include "../res/id.hpp"

static constexpr arch_t kTotalOptions = 2;
static const glm::vec2 kDefaultPosition = glm::vec2(4.0f, 2.0f);

wgt_video_t::wgt_video_t(arch_t flags) :
	widget_i(flags),
	cursor(0),
	text(),
	arrow()
{

}

void wgt_video_t::init(const input_t&, const video_t& video, audio_t&, const music_t&, kernel_t&) {
	ready = true;
	text.set_font(vfs::font(0));
	text.set_position(kDefaultPosition);
	this->setup_text(video.get_parameters());
	arrow.set_file(vfs::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		text.get_font_size().x, 
		4.0f + kDefaultPosition.y + 
		(text.get_font_size().y * 2.0f)
	);
}

void wgt_video_t::handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t&, kernel_t&, stack_gui_t&, draw_headsup_t&) {
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
	} else if (input.pressed[btn_t::Right] or input.pressed[btn_t::Left]) {
		screen_params_t params = video.get_parameters();
		switch (cursor) {
			case 0: {
				params.full = !params.full;
				config.set("Video", "Fullscreen", params.full);
				this->setup_text(params);
				break;
			}
			case 1: {
				if (input.pressed[btn_t::Right]) {
					params.scaling = glm::clamp(
						params.scaling + 1, 
						screen_params_t::kDefaultScaling, 
						screen_params_t::kHighestScaling
					);
				} else {
					params.scaling = glm::clamp(
						params.scaling - 1, 
						screen_params_t::kDefaultScaling, 
						screen_params_t::kHighestScaling
					);
				}
				config.set("Video", "ScaleFactor", params.scaling);
				this->setup_text(params);
				break;
			}
			case 2: {
				params.vsync = !params.vsync;
				config.set("Video", "VerticalSync", params.vsync);
				this->setup_text(params);
				break;
			}
			default: {
				break;
			}
		}
		video.set_parameters(params);
	} else if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
	}
	if (!active) {
		input.pressed.reset();
		audio.play(res::sfx::Inven, 0);
	}
}

void wgt_video_t::update(real64_t delta) {
	if (ready and active) {
		arrow.update(delta);
	}
}

void wgt_video_t::render(renderer_t& renderer) const {
	if (ready and active) {
		text.render(renderer);
		arrow.render(renderer);
	}
}

void wgt_video_t::force() const {
	if (ready and active) {
		text.force();
		arrow.force();
	}
}

void wgt_video_t::setup_text(const screen_params_t& params) {
	std::string data;
	data += vfs::i18n_find("Video", 0, 1);
	data += params.full ? vfs::i18n_find("Main", 1) : vfs::i18n_find("Main", 2);
	data += '\n';
	data += vfs::i18n_find("Video", 2);
	data += std::to_string(params.scaling);
	data += '\n';
	data += vfs::i18n_find("Video", 3);
	data += params.vsync ? vfs::i18n_find("Main", 1) : vfs::i18n_find("Main", 2);
	text.set_string(data);
}