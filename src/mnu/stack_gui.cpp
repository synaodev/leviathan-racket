#include "./stack_gui.hpp"
#include "./wgt_option.hpp"
#include "./wgt_file.hpp"
#include "./wgt_input.hpp"
#include "./wgt_video.hpp"
#include "./wgt_audio.hpp"
#include "./wgt_language.hpp"
#include "./wgt_field.hpp"
#include "./inventory_gui.hpp"

#include "../sys/input.hpp"
#include "../sys/video.hpp"
#include "../sys/audio.hpp"
#include "../sys/renderer.hpp"
#include "../sys/kernel.hpp"

#include "../oly/draw_title_view.hpp"

stack_gui_t::stack_gui_t() :
	write(true),
	release(false),
	widgets()
{

}

void stack_gui_t::reset() {
	write = true;
	release = false;
	widgets.clear();
}

void stack_gui_t::handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, draw_title_view_t& title_view, draw_headsup_t& headsup) {
	if (!widgets.empty()) {
		if (!widgets.back()->is_ready()) {
			widgets.back()->init(input, video, audio, music, kernel);
			title_view.set_head();
		}
		widgets.back()->handle(
			config, input, 
			video, audio,
			music, kernel,
			*this, headsup
		);
		if (release) {
			release = false;
			widgets.clear();
		} else if (!widgets.back()->is_active()) {
			widgets.pop_back();
			if (!widgets.empty()) {
				widgets.back()->force();
			}
		}
	} else if (!kernel.has(kernel_state_t::Lock)) {
		if (input.pressed[btn_t::Options]) {
			widgets.emplace_back(std::make_unique<wgt_option_t>(0));
		}
	}
}

void stack_gui_t::update(real64_t delta) {
	if (!widgets.empty()) {
		widgets.back()->update(delta);
	}
}

void stack_gui_t::render(renderer_t& renderer, const inventory_gui_t& inventory_gui) const {
	if (!widgets.empty()) {
		widgets.back()->render(renderer);
	}
	if (!widgets.empty() or inventory_gui.open()) {
		auto& batch = renderer.get_overlay_quads(
			layer_value::HeadsUp,
			blend_mode_t::Alpha,
			pipeline_t::VtxBlankColors
		);
		if (write) {
			write = false;
			batch.begin(quad_batch_t::SingleQuad)
				.vtx_blank_write(
					rect_t(0.0f, 0.0f, 320.0f, 180.0f), 
					glm::vec4(0.0f, 0.0f, 0.0f, 0.5f)
				)
			.end();
		} else {
			batch.skip(quad_batch_t::SingleQuad);
		}
	}
}

void stack_gui_t::force() const {
	write = true;
}

void stack_gui_t::push(menu_t type, arch_t flags) {
	write = true;
	switch (type) {
	case menu_t::Option:
		widgets.emplace_back(std::make_unique<wgt_option_t>(flags));
		break;
	case menu_t::File:
		widgets.emplace_back(std::make_unique<wgt_file_t>(flags));
		break;
	case menu_t::Input:
		widgets.emplace_back(std::make_unique<wgt_input_t>(flags));
		break;
	case menu_t::Video:
		widgets.emplace_back(std::make_unique<wgt_video_t>(flags));
		break;
	case menu_t::Audio:
		widgets.emplace_back(std::make_unique<wgt_audio_t>(flags));
		break;
	case menu_t::Language:
		widgets.emplace_back(std::make_unique<wgt_language_t>(flags));
		break;
	case menu_t::Field:
		widgets.emplace_back(std::make_unique<wgt_field_t>(flags));
		break;
	default:
		break;
	}
}

void stack_gui_t::pop() {
	if (!widgets.empty()) {
		widgets.pop_back();
	}
}

void stack_gui_t::clear() {
	if (!widgets.empty()) {
		release = true;
	}
}

bool stack_gui_t::empty() const {
	return widgets.empty();
}