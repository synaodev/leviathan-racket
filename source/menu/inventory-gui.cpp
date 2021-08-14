#include "./inventory-gui.hpp"
#include "./stack-gui.hpp"
#include "./dialogue-gui.hpp"
#include "./headsup-gui.hpp"

#include "../resource/id.hpp"
#include "../resource/vfs.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/renderer.hpp"
#include "../system/kernel.hpp"
#include "../system/receiver.hpp"
#include "../utility/logger.hpp"

namespace {
	constexpr arch_t kWrongItem = (arch_t)-1;
	constexpr arch_t kTotalItem = 30;
	constexpr sint_t kMaxInvenX = 5;
	constexpr sint_t kMaxInvenY = 4;
	constexpr arch_t kModulator = 6;

	const glm::vec2 kTopLeftElement 	{ 2.0f, 2.0f };
	const glm::vec2 kElementOffset 		{ 38.0f, 10.0f };
	const glm::ivec2 kElementSpacing 	{ 49, 21 };
	const rect_t kElementBounding		{ 0.0f, 0.0f, 32.0f, 16.0f };
	const rect_t kCountBounding 		{ 56.0f, 9.0f, 8.0f, 9.0f };
}

void inventory_gui_element_t::init(const texture_t* texture, const animation_t* animation, arch_t index) {
	glm::vec2 position {
		kTopLeftElement.x + static_cast<real_t>((index % kModulator) * kElementSpacing.x),
		kTopLeftElement.y + static_cast<real_t>((index / kModulator) * kElementSpacing.y)
	};
	scheme.set_file(animation);
	scheme.set_position(position);
	count.set_texture(texture);
	count.set_position(position + kElementOffset);
	count.set_bounding(kCountBounding);
	count.set_backwards(true);
}

void inventory_gui_element_t::reset(const glm::ivec4& item) {
	if (item.x != 0) {
		visible = true;
		scheme.set_direction(
			static_cast<direction_t>(item.x - 1) // TODO: Static anaylsis doesn't like this
		);
		scheme.invalidate();
		if ((item.y > 1 and !item.w) or (item.w != 0)) {
			count.set_visible(true);
			count.set_value(item.y);
		} else {
			count.set_visible(false);
		}
		count.invalidate();
	} else {
		visible = false;
	}
}

void inventory_gui_element_t::update(real64_t delta) {
	if (visible) {
		scheme.update(delta);
	}
}

void inventory_gui_element_t::render(renderer_t& renderer) const {
	if (visible) {
		scheme.render(renderer);
		count.render(renderer);
	}
}

bool inventory_gui_t::init() {
	const texture_t* texture = vfs_t::texture(res::img::Heads);
	const animation_t* animation = vfs_t::animation(res::anim::Items);
	if (!texture or !animation) {
		synao_log("Inventory GUI is missing resources and cannot be rendered!\n");
		return false;
	}
	current = kWrongItem;
	elements.resize(kTotalItem);
	arch_t index = 0;
	for (auto&& element : elements) {
		element.init(texture, animation, index);
		++index;
	}
	synao_log("Inventory GUI is ready.\n");
	return true;
}

void inventory_gui_t::handle(const input_t& input, audio_t& audio, kernel_t& kernel, receiver_t& receiver, const stack_gui_t& stack_gui, const dialogue_gui_t& dialogue_gui, headsup_gui_t& headsup_gui) {
	if (stack_gui.empty()) {
		if (!ready) {
			if (input.pressed[btn_t::Inventory] and !receiver.running()) {
				amend = true;
				ready = true;
				audio.play(res::sfx::Inven, 0);
				kernel.freeze();
				for (arch_t it = 0; it < elements.size(); ++it) {
					elements[it].reset(kernel.get_item_at(it));
				}
				headsup_gui.set_field_text();
			}
		} else if (input.pressed[btn_t::Inventory]) {
			if (!receiver.running()) {
				amend = true;
				ready = false;
				audio.play(res::sfx::Inven, 0);
				kernel.unlock();
			}
		} else if (!dialogue_gui.get_flag(dialogue_gui_t::Textbox)) {
			glm::ivec2 cursor = kernel.get_cursor();
			if (input.pressed[btn_t::Jump] or input.pressed[btn_t::Hammer]) {
				if (!receiver.running()) {
					arch_t cursor_index = kernel.get_cursor_index();
					glm::ivec4 item_data = kernel.get_item_at(cursor_index);
					arch_t item_type = static_cast<arch_t>(item_data.x);
					receiver.run_inventory(item_type, cursor_index);
					audio.play(res::sfx::TitleBeg, 0);
				}
			} else if (input.pressed[btn_t::Right]) {
				if (cursor.x < kMaxInvenX) {
					amend = true;
					++cursor.x;
					audio.play(res::sfx::Select, 0);
				}
			} else if (input.pressed[btn_t::Left]) {
				if (cursor.x > 0) {
					amend = true;
					--cursor.x;
					audio.play(res::sfx::Select, 0);
				}
			} else if (input.pressed[btn_t::Up]) {
				if (cursor.y > 0) {
					amend = true;
					--cursor.y;
					audio.play(res::sfx::Select, 0);
				}
			} else if (input.pressed[btn_t::Down]) {
				if (cursor.y < kMaxInvenY) {
					amend = true;
					++cursor.y;
					audio.play(res::sfx::Select, 0);
				}
			}
			kernel.set_cursor(cursor);
		}
	}
}

void inventory_gui_t::update(real64_t delta) {
	if (ready) {
		for (auto&& element : elements) {
			element.update(delta);
		}
	}
}

void inventory_gui_t::render(renderer_t& renderer, const kernel_t& kernel) const {
	if (ready) {
		for (auto&& element : elements) {
			element.render(renderer);
		}
		auto& list = renderer.display_list(
			layer_value::Persistent,
			blend_mode_t::Alpha,
			program_t::Colors
		);
		if (amend) {
			amend = false;
			// glm::vec2 cursor_position = 2.0f + glm::vec2(
			// 	glm::ivec2(49, 21) * kernel.get_cursor()
			// );
			glm::vec2 cursor_position = kTopLeftElement + glm::vec2(kElementSpacing * kernel.get_cursor());
			list.begin(display_list_t::SingleQuad)
				.vtx_blank_write(
					kElementBounding,
					{ 0.0f, 0.0f, 1.0f, 0.5f } // Blue
				)
				.vtx_transform_write(cursor_position)
			.end();
		} else {
			list.skip(display_list_t::SingleQuad);
		}
		arch_t item_index = kernel.get_item_ptr_index();
		if (item_index < kTotalItem) {
			if (current != item_index) {
				current = item_index;
				// glm::vec2 cursor_position = 2.0f + glm::vec2(
				// 	static_cast<real_t>((item_index % kModulator) * 49),
				// 	static_cast<real_t>((item_index / kModulator) * 21)
				// );
				glm::vec2 cursor_position {
					kTopLeftElement.x + static_cast<real_t>((item_index % kModulator) * kElementSpacing.x),
					kTopLeftElement.y + static_cast<real_t>((item_index / kModulator) * kElementSpacing.y)
				};
				list.begin(display_list_t::SingleQuad)
					.vtx_blank_write(
						kElementBounding,
						{ 1.0f, 0.0f, 0.0f, 0.5f } // Red
					)
					.vtx_transform_write(cursor_position)
				.end();
			} else {
				list.skip(display_list_t::SingleQuad);
			}
		}
	}
}

bool inventory_gui_t::open() const {
	return ready;
}
