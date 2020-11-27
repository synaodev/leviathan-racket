#include "./inventory-gui.hpp"
#include "./stack-gui.hpp"
#include "./dialogue-gui.hpp"
#include "./headsup-gui.hpp"

#include "../resource/id.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/renderer.hpp"
#include "../system/kernel.hpp"
#include "../system/receiver.hpp"
#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"

static constexpr arch_t kWrongItem = (arch_t)-1;
static constexpr arch_t kTotalItem = 30;
static constexpr sint_t kMaxInvenX = 5;
static constexpr sint_t kMaxInvenY = 4;

inventory_gui_element_t::inventory_gui_element_t() :
	visible(false),
	scheme(),
	count()
{

}

void inventory_gui_element_t::init(const texture_t* texture, const palette_t* palette, const animation_t* animation, arch_t index) {
	glm::vec2 position = glm::vec2(
		2.0f + static_cast<real_t>((index % 6) * 49),
		2.0f + static_cast<real_t>((index / 6) * 21)
	);
	scheme.set_file(animation);
	scheme.set_position(position);
	count.set_texture(texture);
	count.set_palette(palette);
	count.set_position(position + glm::vec2(38.0f, 10.0f));
	count.set_bounding(56.0f, 18.0f, 8.0f, 10.0f);
	count.set_backwards(true);
}

void inventory_gui_element_t::reset(glm::ivec4 item) {
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

inventory_gui_t::inventory_gui_t() :
	amend(true),
	current(kWrongItem),
	ready(false),
	elements(kTotalItem)
{

}

bool inventory_gui_t::init() {
	const texture_t* texture = vfs::texture(res::img::Heads);
	const palette_t* palette = vfs::palette(res::pal::Heads);
	const animation_t* animation = vfs::animation(res::anim::Items);
	if (!texture or !palette or !animation) {
		synao_log("Inventory GUI is missing resources and cannot be rendered!\n");
		return false;
	}
	arch_t index = 0;
	for (auto&& element : elements) {
		element.init(texture, palette, animation, index);
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
		} else if (!dialogue_gui.get_flag(dialogue_flag_t::Textbox)) {
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
			glm::vec2 cursor_position = 2.0f + glm::vec2(
				glm::ivec2(49, 21) * kernel.get_cursor()
			);
			list.begin(display_list_t::SingleQuad)
				.vtx_blank_write(
					rect_t(0.0f, 0.0f, 32.0f, 16.0f),
					glm::vec4(0.0f, 0.0f, 1.0f, 0.5f)
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
				glm::vec2 cursor_position = 2.0f + glm::vec2(
					static_cast<real_t>((item_index % 6) * 49),
					static_cast<real_t>((item_index / 6) * 21)
				);
				list.begin(display_list_t::SingleQuad)
					.vtx_blank_write(
						rect_t(0.0f, 0.0f, 32.0f, 16.0f),
						glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)
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
