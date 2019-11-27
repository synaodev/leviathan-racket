#include "./inventory_gui.hpp"
#include "./stack_gui.hpp"
#include "./dialogue_gui.hpp"

#include "../utl/vfs.hpp"

#include "../eve/receiver.hpp"

#include "../oly/draw_title_view.hpp"

#include "../sys/input.hpp"
#include "../sys/audio.hpp"
#include "../sys/renderer.hpp"
#include "../sys/kernel.hpp"

#include "../res_id.hpp"

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

void inventory_gui_element_t::init(const texture_t* texture, const animation_t* animation, arch_t index) {
	glm::vec2 position = glm::vec2(
		2.0f + static_cast<real_t>((index % 6) * 49), 
		2.0f + static_cast<real_t>((index / 6) * 21)
	);
	scheme.set_file(animation);
	scheme.set_state(7);
	scheme.set_position(position);
	count.set_texture(texture);
	count.set_position(position + glm::vec2(38.0f, 10.0f));
	count.set_bounding(156.0f, 18.0f, 8.0f, 10.0f);
	count.set_backwards(true);
}

void inventory_gui_element_t::reset(glm::ivec4 item) {
	if (item.x != 0) {
		visible = true;
		scheme.set_direction(
			static_cast<direction_t>(item.x - 1)
		);
		if ((item.y > 1 and !item.w) or (item.w != 0)) {
			count.set_visible(true);
			count.set_value(item.y);
		} else {
			count.set_visible(false);
		}
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
	write(true),
	current(kWrongItem),
	ready(false),
	elements(kTotalItem)
{

}

bool inventory_gui_t::init() {
	const texture_t* texture = vfs::texture(res::img::Heads);
	const animation_t* animation = vfs::animation(res::anim::Heads);
	if (texture == nullptr or animation == nullptr) {
		return false;
	}
	arch_t index = 0;
	for (auto&& element : elements) {
		element.init(texture, animation, index);
		++index;
	}
	return true;
}

void inventory_gui_t::handle(const input_t& input, audio_t& audio, kernel_t& kernel, receiver_t& receiver, const stack_gui_t& stack_gui, const dialogue_gui_t& dialogue_gui, draw_title_view_t& title_view) {
	if (stack_gui.empty()) {
		if (!ready) {
			if (input.pressed[btn_t::Inventory] and !receiver.running()) {
				write = true;
				ready = true;
				audio.play(res::sfx::Inven, 0);
				kernel.freeze();
				for (arch_t it = 0; it < elements.size(); ++it) {
					elements[it].reset(kernel.get_item_at(it));
				}
				title_view.set_head();
			}
		} else if (input.pressed[btn_t::Inventory]) {
			if (!receiver.running()) {
				write = true;
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
					write = true;
					++cursor.x;
					audio.play(res::sfx::Select, 0);
				}
			} else if (input.pressed[btn_t::Left]) {
				if (cursor.x > 0) {
					write = true;
					--cursor.x;
					audio.play(res::sfx::Select, 0);
				}
			} else if (input.pressed[btn_t::Up]) {
				if (cursor.y > 0) {
					write = true;
					--cursor.y;
					audio.play(res::sfx::Select, 0);
				}
			} else if (input.pressed[btn_t::Down]) {
				if (cursor.y < kMaxInvenY) {
					write = true;
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
		auto& batch = renderer.get_overlay_quads(
			layer_value::HeadsUp,
			blend_mode_t::Alpha,
			render_pass_t::VtxBlankColors
		);
		if (write) {
			write = false;
			glm::vec2 cursor_position = 2.0f + glm::vec2(
				glm::ivec2(49, 21) * kernel.get_cursor()
			);
			batch.begin(quad_batch_t::SingleQuad)
				.vtx_blank_write(
					rect_t(0.0f, 0.0f, 32.0f, 16.0f), 
					glm::vec4(0.0f, 0.0f, 1.0f, 0.5f)
				)
				.vtx_transform_write(cursor_position)
			.end();
		} else {
			batch.skip(quad_batch_t::SingleQuad);
		}
		arch_t item_index = kernel.get_item_ptr_index();
		if (item_index < kTotalItem) {
			if (current != item_index) {
				current = item_index;
				glm::vec2 cursor_position = 2.0f + glm::vec2(
					static_cast<real_t>((item_index % 6) * 49),
					static_cast<real_t>((item_index / 6) * 21)
				);
				batch.begin(quad_batch_t::SingleQuad)
					.vtx_blank_write(
						rect_t(0.0f, 0.0f, 32.0f, 16.0f), 
						glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)
					)
					.vtx_transform_write(cursor_position)
				.end();
			} else {
				batch.skip(quad_batch_t::SingleQuad);
			}
		}
	}
}

bool inventory_gui_t::open() const {
	return ready;
}