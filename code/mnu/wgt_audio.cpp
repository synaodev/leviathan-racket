#include "./wgt_audio.hpp"

#include "../utl/vfs.hpp"
#include "../utl/setup_file.hpp"
#include "../sym/input.hpp"
#include "../sym/audio.hpp"
#include "../sym/music.hpp"

#include "../res/id.hpp"

static constexpr arch_t kTotalOptions = 2;
static const glm::vec2 kDefaultPosition = glm::vec2(4.0f, 2.0f);

wgt_audio_t::wgt_audio_t(arch_t flags) :
	widget_i(flags),
	cursor(0),
	text(),
	arrow()
{

}

void wgt_audio_t::init(const input_t&, const video_t&, audio_t& audio, const music_t& music, kernel_t&) {
	ready = true;
	text.set_font(vfs::font(0));
	text.set_position(kDefaultPosition);
	this->setup_text(audio, music);
	arrow.set_file(vfs::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		text.get_font_size().x, 
		4.0f + kDefaultPosition.y + 
		(text.get_font_size().y * 2.0f)
	);
}

void wgt_audio_t::handle(setup_file_t& config, input_t& input, video_t&, audio_t& audio, music_t& music, kernel_t&, stack_gui_t&, draw_headsup_t&) {
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
	} else if (input.holding[btn_t::Right] or input.holding[btn_t::Left]) {
		switch (cursor) {
			case 0: {
				real_t volume = audio.get_volume();
				if (input.holding[btn_t::Right]) {
					volume = glm::clamp(volume + 0.01f, 0.0f, 1.0f);
				} else {
					volume = glm::clamp(volume - 0.01f, 0.0f, 1.0f);
				}
				audio.set_volume(volume);
				config.set("Audio", "Volume", volume);
				this->setup_text(audio, music);
				break;
			}
			case 1: {
				real_t volume = music.get_volume();
				if (input.holding[btn_t::Right]) {
					volume = glm::clamp(volume + 0.01f, 0.0f, 1.0f);
				} else {
					volume = glm::clamp(volume - 0.01f, 0.0f, 1.0f);
				}
				music.set_volume(volume);
				config.set("Music", "Volume", volume);
				this->setup_text(audio, music);
				break;
			}
			default: {
				break;
			}
		}
	} else if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
	}
	if (!active) {
		input.pressed.reset();
		audio.play(res::sfx::Inven, 0);
	}
}

void wgt_audio_t::update(real64_t delta) {
	if (ready and active) {
		arrow.update(delta);
	}
}

void wgt_audio_t::render(renderer_t& renderer) const {
	if (ready and active) {
		text.render(renderer);
		arrow.render(renderer);
	}
}

void wgt_audio_t::force() const {
	if (ready and active) {
		text.force();
		arrow.force();
	}
}

void wgt_audio_t::setup_text(const audio_t& audio, const music_t& music) {
	std::string data;
	data += vfs::i18n_find("Audio", 0, 1);
	data += std::to_string(audio.get_volume());
	data += '\n';
	data += vfs::i18n_find("Audio", 2);
	data += std::to_string(music.get_volume());
	data += '\n';
	text.set_string(data);
}