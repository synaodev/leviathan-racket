#include "./wgt-video.hpp"

#include "../resource/config.hpp"
#include "../resource/id.hpp"
#include "../resource/vfs.hpp"
#include "../system/input.hpp"
#include "../system/video.hpp"
#include "../system/audio.hpp"

#include <glm/common.hpp>
#include <fmt/format.h>

namespace {
	constexpr arch_t kVideoTotalOptions 	= 2;
	const glm::vec2 kVideoDefaultPosition 	{ 4.0f, 2.0f };
}

void wgt_video_t::init(const input_t&, const video_t& video, audio_t&, const music_t&, kernel_t&) {
	ready = true;
	text.set_font(vfs_t::font(0));
	text.set_position(kVideoDefaultPosition);
	this->setup_text(video.get_parameters());
	arrow.set_file(vfs_t::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		text.get_font_size().x,
		4.0f + kVideoDefaultPosition.y +
		(text.get_font_size().y * 2.0f)
	);
}

void wgt_video_t::handle(config_t& config, input_t& input, video_t& video, audio_t& audio, music_t&, kernel_t&, stack_gui_t&, headsup_gui_t&) {
	if (input.pressed[btn_t::Up]) {
		if (cursor > 0) {
			--cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, -text.get_font_size().y);
		} else {
			cursor = kVideoTotalOptions;
			audio.play(res::sfx::Select, 0);
			arrow.set_position(
				text.get_font_size().x,
				(4.0f + kVideoDefaultPosition.y) +
				(text.get_font_size().y * 2.0f) +
				(text.get_font_size().y * static_cast<real_t>(kVideoTotalOptions))
			);
		}
	} else if (input.pressed[btn_t::Down]) {
		if (cursor < kVideoTotalOptions) {
			++cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, text.get_font_size().y);
		} else {
			cursor = 0;
			audio.play(res::sfx::Select, 0);
			arrow.set_position(
				text.get_font_size().x,
				4.0f + kVideoDefaultPosition.y +
				(text.get_font_size().y * 2.0f)
			);
		}
	} else if (input.pressed[btn_t::Right] or input.pressed[btn_t::Left]) {
		auto params = video.get_parameters();
		switch (cursor) {
			case 0: {
				params.full = !params.full;
				config.set_fullscreen(params.full);
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
				config.set_scaling(params.scaling);
				this->setup_text(params);
				break;
			}
			case 2: {
				params.vsync = !params.vsync;
				config.set_vertical_sync(params.vsync);
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
		input.flush();
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

void wgt_video_t::invalidate() const {
	if (ready and active) {
		text.invalidate();
		arrow.invalidate();
	}
}

void wgt_video_t::setup_text(const screen_params_t& params) {
	fmt::memory_buffer data {};
	fmt::format_to(data, "{}{}\n{}{}\n{}{}",
	 	vfs_t::i18n_find("Video", 0, 1),
	 	params.full ? vfs_t::i18n_find("Main", 1) : vfs_t::i18n_find("Main", 2),
	 	vfs_t::i18n_find("Video", 2),
	 	params.scaling,
	 	vfs_t::i18n_find("Video", 3),
	 	params.vsync ? vfs_t::i18n_find("Main", 1) : vfs_t::i18n_find("Main", 2)
	);
	text.set_string(fmt::to_string(data));
}
