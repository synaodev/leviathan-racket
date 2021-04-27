#include "./stack-gui.hpp"
#include "./inventory-gui.hpp"
#include "./headsup-gui.hpp"
#include "./wgt-option.hpp"
#include "./wgt-file.hpp"
#include "./wgt-input.hpp"
#include "./wgt-video.hpp"
#include "./wgt-audio.hpp"
#include "./wgt-language.hpp"

#include "../utility/logger.hpp"
#include "../system/input.hpp"
#include "../system/video.hpp"
#include "../system/audio.hpp"
#include "../system/renderer.hpp"
#include "../system/kernel.hpp"

void stack_gui_t::reset() {
	amend = true;
	release = false;
	widgets.clear();
}

void stack_gui_t::handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, headsup_gui_t& headsup_gui) {
	if (!widgets.empty()) {
		if (!widgets.back()->is_ready()) {
			widgets.back()->init(input, video, audio, music, kernel);
			headsup_gui.set_field_text();
		}
		widgets.back()->handle(
			config, input,
			video, audio,
			music, kernel,
			*this, headsup_gui
		);
		if (release) {
			release = false;
			widgets.clear();
		} else if (!widgets.back()->is_active()) {
			widgets.pop_back();
			if (!widgets.empty()) {
				widgets.back()->invalidate();
			}
		}
	} else if (!kernel.has(kernel_t::Lock)) {
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
		auto& list = renderer.display_list(
			layer_value::Persistent,
			blend_mode_t::Alpha,
			program_t::Colors
		);
		if (amend) {
			amend = false;
			list.begin(display_list_t::SingleQuad)
				.vtx_blank_write(
					rect_t(0.0f, 0.0f, 320.0f, 180.0f),
					glm::vec4(0.0f, 0.0f, 0.0f, 0.5f)
				)
			.end();
		} else {
			list.skip(display_list_t::SingleQuad);
		}
	}
}

void stack_gui_t::invalidate() const {
	amend = true;
}

void stack_gui_t::push(menu_t type, arch_t flags) {
	amend = true;
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
	default:
		synao_log("Widget type invalid! This code block should't run!\n");
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
